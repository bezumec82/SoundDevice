#include "main.h"

/* Global variables */
mcasp_ts pru0_mcasp0_ts0 (PRU0, MCASP0, TS0);
mcasp_ts pru0_mcasp0_ts1 (PRU0, MCASP0, TS1);
mcasp_ts pru1_mcasp1_ts0 (PRU1, MCASP1, TS0);
mcasp_ts pru1_mcasp1_ts1 (PRU1, MCASP1, TS1);
#if (!C_STYLE)
usb_gadget usb;
#endif
/*----------------------------------------------------------------------------*/

#if (RESAMPLE_8k_to_16k)
/*!
 * Return pointer to the buffer of int32_t re-sampled values.
 * @param pckg_buf_ptr
 * @param pckg_BUF_SIZE_4ms_BYTES
 * @return
 */
static int8_t last_data = 0;
int32_t * resample_8x8000_to_16x16000   (
                                        int8_t   * pckg_buf_ptr,
                                        int&  pckg_BUF_SIZE_4ms_BYTES
                                        )
{
    int32_t * resampled_buf_ptr = (int32_t *)malloc(pckg_BUF_SIZE_4ms_BYTES * (sizeof(int32_t)/sizeof(int8_t)) * (16000/8000));
    if(!resampled_buf_ptr) { FPRINTF; exit(-EXIT_FAILURE); }

    int resample_koeff = 32768/256;
    for (int i = 0,j = 0; i < (pckg_BUF_SIZE_4ms_BYTES); i+=1, j+=2)
    {
        if (i == 0)
        {
        resampled_buf_ptr[j]   = ((last_data + pckg_buf_ptr[i]) * resample_koeff)/2;
        resampled_buf_ptr[j+1] =              (pckg_buf_ptr[i]  * resample_koeff)  ;
        continue;
        }
        resampled_buf_ptr[j]   = ((pckg_buf_ptr[i-1] + pckg_buf_ptr[i]) * resample_koeff)/2;
        resampled_buf_ptr[j+1] =                      (pckg_buf_ptr[i]  * resample_koeff)  ;
    }

    last_data = pckg_buf_ptr[pckg_BUF_SIZE_4ms_BYTES - 1];
    pckg_BUF_SIZE_4ms_BYTES = (pckg_BUF_SIZE_4ms_BYTES * (sizeof(int32_t)/sizeof(int8_t)) * (16000/8000));
    return resampled_buf_ptr;
}
/*----------------------------------------------------------------------------*/
#endif

#if (C_STYLE)
/* Global variables */
static struct io_thread_args thread_args;
pthread_t io_thread;
static int io_thread_started = 0;
/*----------------------------------------------------------------------------*/

/*
 * Respond to host requests
 */
static void * io_thread_function(void * arg)
{
    struct io_thread_args * thread_args_ptr = (struct io_thread_args*)arg;
    struct timeval timeout;
    int ret, max_read_fd, max_write_fd;
    uint8_t pckg_buf[4096];
    max_read_fd = max_write_fd = 0;
    if (thread_args_ptr->to_dev_ep_fd >  max_write_fd) { max_write_fd = thread_args_ptr->to_dev_ep_fd;  }
    if (thread_args_ptr->to_hst_ep_fd > max_read_fd)  { max_read_fd  = thread_args_ptr->to_hst_ep_fd; }
    while (!thread_args_ptr->stop)
    {
        /*------------*/
        /* READ block */
        /*------------*/
    #if (1)
        fd_set read_set = { 0 };
        FD_ZERO(&read_set);
        FD_SET(thread_args_ptr->to_dev_ep_fd, &read_set);
        timeout.tv_usec = SELECT_TIMEOUT_MCSEC; // 10ms
        ret = select(
                    max_read_fd + 1,
                    &read_set,
                    NULL,
                    NULL,
                    &timeout
                    );
        if (ret == 0) { continue; }       /* Timeout */
        if (ret < 0)  { FPRINTF; break; } /* Error */
        bzero(pckg_buf, sizeof(pckg_buf));
        ret = read (thread_args_ptr->to_dev_ep_fd, pckg_buf, sizeof(pckg_buf));
        if (ret > 0)
        {
#if RESAMPLE_8k_to_16k
        PRINTF(GRN, "%f : Read %d bytes.", Get_timestamp_ms(), ret);
        int32_t * resampled_buf_ptr = resample_8x8000_to_16x16000((int8_t *)pckg_buf, ret);
        printf(GRN "Write %d bytes.\n", ret);
        pru0_mcasp0_ts0.write_mcasp((int32_t *)&resampled_buf_ptr[0], uint32_t(ret));
        pru0_mcasp0_ts1.write_mcasp((int32_t *)&resampled_buf_ptr[0], uint32_t(ret));
        pru1_mcasp1_ts0.write_mcasp((int32_t *)&resampled_buf_ptr[0], uint32_t(ret));
        pru1_mcasp1_ts1.write_mcasp((int32_t *)&resampled_buf_ptr[0], uint32_t(ret));
        free(resampled_buf_ptr);
#else
        PRINTF(GRN, "%f ms : Read %i bytes\n.", Get_timestamp_ms(), ret);
        pru0_mcasp0_ts0.write_mcasp((int32_t *)&pckg_buf[0], uint32_t(ret));
        pru0_mcasp0_ts1.write_mcasp((int32_t *)&pckg_buf[0], uint32_t(ret));
        pru1_mcasp1_ts0.write_mcasp((int32_t *)&pckg_buf[0], uint32_t(ret));
        pru1_mcasp1_ts1.write_mcasp((int32_t *)&pckg_buf[0], uint32_t(ret));
#endif
        }
        else { PRINT_ERR("Read error %d(%m).\n", ret); }
    #endif
        /*-------------*/
        /* Write block */
        /*-------------*/
    #if (0)
        fd_set write_set = { 0 };
        FD_ZERO(&write_set);
        FD_SET(thread_args_ptr->to_hst_ep_fd, &write_set);
        ret = select(
                max_write_fd + 1,
                NULL,
                &write_set,
                NULL,
                NULL
                );
        if (ret < 0) { FPRINTF; break; } /* Error */
        bzero(pckg_buf, sizeof(pckg_buf));
        strcpy(pckg_buf, "microElk");
        ret = write (thread_args_ptr->to_hst_ep_fd, pckg_buf, strlen(buffer) + 1);
        if (ret > 0) { PRINTF(GRN, "Written %d bytes.\n", ret); }
        else         { PRINT_ERR("Write error %d(%m).\n", ret); }
    #endif
    }
    close (thread_args_ptr->to_dev_ep_fd);
    close (thread_args_ptr->to_hst_ep_fd);
    thread_args_ptr->to_dev_ep_fd = -1;
    thread_args_ptr->to_hst_ep_fd = -1;
    return NULL;
}
/*----------------------------------------------------------------------------*/

static int init_ep  (
                    int * to_dev_ep_fd,
                    int * to_hst_ep_fd
                    )
{
    uint8_t       init_config[2048];
    * (uint32_t *)init_config = 0x00000001;
    uint8_t *     cfg_ptr     = &init_config[0];
    int           ret_val     = -1;
    uint32_t      send_size   = 0;
    uint32_t      tag         = 0x00000001;

    /*!
     * Configure ep2 toward device (read)
     */
#if (1)
    * to_dev_ep_fd = open(USB_EPOUT, O_RDWR);
    if (* to_dev_ep_fd <= 0)
    {
        FPRINTF_EXT("Unable to open %s\n", USB_EPOUT);
        goto error;
    }
    #if (0)
        * (uint32_t *)init_config = 1;
        cfg_ptr = &init_config[4];
        FETCH(to_dev_ep_descriptor);
        FETCH(to_dev_ep_descriptor);
        send_size = (uint32_t)cfg_ptr - (uint32_t)init_config; /* Address arithmetics */
    #else
        /* Prepare config */
        cfg_ptr = &init_config[0];
        memcpy(cfg_ptr, &tag, sizeof(tag));
        cfg_ptr += sizeof(tag);
        send_size += sizeof(tag);
        memcpy(cfg_ptr, &to_dev_ep_descriptor, to_dev_ep_descriptor.bLength);
        cfg_ptr += to_dev_ep_descriptor.bLength;
        send_size += to_dev_ep_descriptor.bLength;
        memcpy(cfg_ptr, &to_dev_ep_descriptor, to_dev_ep_descriptor.bLength);
        cfg_ptr += to_dev_ep_descriptor.bLength;
        send_size += to_dev_ep_descriptor.bLength;
    #endif
    ret_val = write(* to_dev_ep_fd, init_config, send_size);
    if (ret_val != (int)send_size)
    {
        PRINT_ERR("Write error %d (%m)\n", ret_val);
        goto error;
    }
    PRINTF(GRN, "ep2(%s) is configured\n", USB_EPOUT);
#endif
    /*!
     * Configure ep1 toward host (write)
     */
#if (1)
    * to_hst_ep_fd = open(USB_EPIN, O_RDWR);
    if (* to_hst_ep_fd <= 0)
    {
        PRINT_ERR("Unable to open %s (%m)\n", USB_EPIN);
        goto error;
    }
    #if (0)
        * (uint32_t *)init_config = 1;
        cfg_ptr = &init_config[4];
        FETCH(to_hst_ep_descriptor);
        FETCH(to_hst_ep_descriptor);
        send_size = (uint32_t)cfg_ptr - (uint32_t)init_config; /* Address arithmetics */
    #else
        cfg_ptr = &init_config[0];
        memcpy(cfg_ptr, &tag, sizeof(tag));
        cfg_ptr += sizeof(tag);
        memcpy(cfg_ptr, &to_hst_ep_descriptor, to_hst_ep_descriptor.bLength);
        cfg_ptr += to_hst_ep_descriptor.bLength;
        memcpy(cfg_ptr, &to_hst_ep_descriptor, to_hst_ep_descriptor.bLength);
        cfg_ptr += to_hst_ep_descriptor.bLength;
        send_size = (uint32_t)cfg_ptr - (uint32_t)init_config; /* Address arithmetics */
    #endif
    ret_val = write(* to_hst_ep_fd, init_config, send_size);
    if (ret_val != (int)send_size)
    {
        PRINT_ERR("Write error %d (%m)\n", ret_val);
        goto error;
    }
    PRINTF(GRN, "ep1(%s) is configured\n", USB_EPIN);
#endif
    ret_val = 0;
    error: return ret_val;
}
/*----------------------------------------------------------------------------*/

static void handle_setup_request (int ep0_fd, struct usb_ctrlrequest * setup)
{
    int status;
    uint8_t buffer[512];

    PRINTF(YEL, "Handling setup request.\n");
    switch (setup->bRequest)
    {
        case USB_REQ_GET_DESCRIPTOR:
        {
        #if (1)
            PRINTF(BLU,"GET_DESCRIPTOR (%d) request.\n", setup->bRequest);
            if (setup->bRequestType != USB_DIR_IN)
            {
                PRINT_ERR("Bad direction : 0x%x.\n", setup->bRequestType);
                goto stall;
            }
            switch (setup->wValue >> 8)
            {
                case USB_DT_STRING:
                {
                    PRINTF(CYN, "Request STRING (0x%x). Descriptor index : %d (max length %d)\n",
                           (setup->wValue >> 8), setup->wValue & 0xff, setup->wLength);
                    {
                        extern int usb_gadget_get_string (struct usb_gadget_strings *table, int id, __u8 *buf);
                        status = usb_gadget_get_string (&strings, setup->wValue & 0xff, buffer);
                    }
                    if (status < 0) /* Error */
                    {
                        PRINT_ERR("String not found.\n");
                        break;
                    }
                    else
                    {
                        PRINTF(GRN, "Found string #%d. bLength = %d.\n", setup->wValue, status);
                    }
                    write (ep0_fd, buffer, status);
                    return;
                } //end case USB_DT_STRING
                default: PRINT_ERR("Can not return descriptor %d\n", (setup->wValue >> 8));
            } //end switch (setup->wValue >> 8)
            break;
        #endif
        } //end USB_REQ_GET_DESCRIPTOR
        case USB_REQ_SET_CONFIGURATION:
        {
        #if (1)
            PRINTF(BLU,"SET_CONFIGURATION (%d) request.\n", setup->bRequest);
            if (setup->bRequestType != USB_DIR_OUT)
            {
                PRINT_ERR("Bad direction.\n");
                goto stall;
            }
            switch (setup->wValue)
            {
                case CONFIG_VALUE:
                {
                    PRINTF(GRN, "Set config value\n");
                    if (!thread_args.stop)
                    {
                        thread_args.stop = 1;
                        if (io_thread_started)
                        {
                        PRINTF(MAG, "Stopping IO thread.\n");
                        io_thread_started = 0;
                        PRINTF(MAG, "Waiting for IO thread termination.\n");
                        pthread_join(io_thread, NULL);
                        }
                        else
                        {
                            PRINTF(MAG, "IO thread is not started yet.\n");
                        } //end if (io_thread_started)
                    } //end if (!thread_args.stop)
                    if (thread_args.to_dev_ep_fd <= 0)
                    {
                        PRINTF(CYN, "Initializing endpoints.\n");
                        status = init_ep (&thread_args.to_dev_ep_fd, &thread_args.to_hst_ep_fd);
                    }
                    else
                    {
                        status = 0;
                    }
                    if (!status)
                    {
                        if (io_thread_started)
                        {
                            PRINT_ERR("IO thread already started.\n");
                        }
                        else
                        {
                        PRINTF(MAG, "Starting IO thread.\n");
                        thread_args.stop = 0;
                        pthread_create(&io_thread, NULL, io_thread_function, &thread_args);
                        io_thread_started = 1;
                        }
                    } //end if (!status)
                    break;
                } //end case CONFIG_VALUE
                case 0:
                {
                    PRINTF(RED, "Disable threads\n");
                    thread_args.stop = 1;
                    break;
                }
                default:
                {
                    PRINT_ERR("Un-handled configuration value %d\n", setup->wValue);
                    break;
                }
            } //end switch
            // Just ACK
            status = read (ep0_fd, &status, 0);
            return;
        #endif
        } //end USB_REQ_SET_CONFIGURATION
        case USB_REQ_GET_INTERFACE:
        {
        #if (1)
            PRINTF(BLU,"GET_INTERFACE request.\n");
            buffer[0] = 0;
            write (ep0_fd, buffer, 1);
            return;
        #endif
        } //end USB_REQ_GET_INTERFACE
        case USB_REQ_SET_INTERFACE:
        {
        #if (1)
            PRINTF(BLU,"SET_INTERFACE request.\n");
            ioctl (thread_args.to_dev_ep_fd, GADGETFS_CLEAR_HALT);
            ioctl (thread_args.to_hst_ep_fd, GADGETFS_CLEAR_HALT);
            // ACK
            status = read (ep0_fd, &status, 0);
            return;
        #endif
        } //end USB_REQ_SET_INTERFACE
        default : { PRINT_ERR("Un-handled setup request %d.\n", setup->bRequest); }
    } //end switch

stall:
    PRINT_ERR("Stalled\n");
    // Error
    if (setup->bRequestType & USB_DIR_IN) { read  (ep0_fd, &status, 0); }
    else                                  { write (ep0_fd, &status, 0); }
}
/*----------------------------------------------------------------------------*/

static void handle_ep0_events   (
                                int ep0_fd,
                                struct usb_gadgetfs_event * event_ptr,
                                int events_ammnt
                                )
{
    for (int i=0; i<events_ammnt; i++)
    {
        switch (event_ptr[i].type)
        {
        #if (1)
            case GADGETFS_CONNECT:
            {
                PRINTF(CYN, "ep0 connect event.\n");
                break;
            }
            case GADGETFS_DISCONNECT:
            {
                PRINTF(CYN, "ep0 disconnect event.\n");
                break;
            }
        #endif
            case GADGETFS_SETUP:
            {
                PRINTF(CYN, "ep0 setup event.\n");
                handle_setup_request(ep0_fd, &event_ptr[i].u.setup);
                break;
            }
            case GADGETFS_NOP: /* Fall through */
            case GADGETFS_SUSPEND: break;
        } //end switch
    } //end for
}
/*----------------------------------------------------------------------------*/

static void handle_ep0(int ep0_fd)
{
    int ret_val, events_ammnt;
    fd_set read_set;
    struct usb_gadgetfs_event events[5];
    while (true)
    {
        FD_ZERO(&read_set);
        FD_SET(ep0_fd, &read_set);
        select(ep0_fd + 1,
               &read_set,
               NULL,
               NULL,
               NULL);
        ret_val = read(ep0_fd, &events, sizeof(events));
        if (ret_val < 0)
        {
            PRINT_ERR("Read error. Returned : %d\n", ret_val);
            goto error;
        }
        events_ammnt = ret_val / sizeof(events[0]);
        PRINTF(MAG, "%d event(s) happened.\n", events_ammnt);
        handle_ep0_events   (
                            ep0_fd,
                            &events[0],
                            events_ammnt
                            );
    } //end while (1)
error: return;
}
/*----------------------------------------------------------------------------*/

/*!
 * A special endpoint called ep0 (the first one) is always present.
 * It's an in and out endpoint with control attribute.
 * It allows to read/write configuration of other endpoints.
 */
int cfg_ep0(int * ep0_fd_ptr)
{
    int       ret_val           = -1;
    uint32_t  send_size         = 0;
    uint8_t   init_config[2048] = { 0 };
    uint8_t * cfg_ptr           = &init_config[0];

    * ep0_fd_ptr = open(USB_DEV, O_RDWR|O_SYNC);
    if (* ep0_fd_ptr <= 0)
    {
        FPRINTF_EXT("Unable to open %s.\n", USB_DEV);
        exit(-EXIT_FAILURE);
    }
    /* Prepare config */
    uint32_t tag = 0x00000000;
    memcpy(cfg_ptr, &tag,                   sizeof(tag));                   cfg_ptr += sizeof(tag);
    memcpy(cfg_ptr, &config,                config.bLength);                cfg_ptr += config.bLength;
    memcpy(cfg_ptr, &interface_descriptor,  interface_descriptor.bLength);  cfg_ptr += interface_descriptor.bLength;
    memcpy(cfg_ptr, &to_hst_ep_descriptor,  to_hst_ep_descriptor.bLength);  cfg_ptr += to_hst_ep_descriptor.bLength;
    memcpy(cfg_ptr, &to_dev_ep_descriptor,  to_dev_ep_descriptor.bLength);  cfg_ptr += to_dev_ep_descriptor.bLength;

    memcpy(cfg_ptr, &config_hs,             config_hs.bLength);             cfg_ptr += config_hs.bLength;
    memcpy(cfg_ptr, &interface_descriptor,  interface_descriptor.bLength);  cfg_ptr += interface_descriptor.bLength;
    memcpy(cfg_ptr, &to_hst_ep_descriptor,  to_hst_ep_descriptor.bLength);  cfg_ptr += to_hst_ep_descriptor.bLength;
    memcpy(cfg_ptr, &to_dev_ep_descriptor,  to_dev_ep_descriptor.bLength);  cfg_ptr += to_dev_ep_descriptor.bLength;

    memcpy(cfg_ptr, &device_descriptor,     device_descriptor.bLength);     cfg_ptr += device_descriptor.bLength;
    send_size = (uint32_t)cfg_ptr-(uint32_t)init_config;

    ret_val = write(* ep0_fd_ptr, init_config, send_size);
    if (ret_val != (int)send_size) { FPRINTF; return -1; }
    PRINTF(GRN, "ep0(%s) is successfully configured\n", USB_DEV);
    return 0;
}
#endif

#define POLL_FDS_AMMNT                  6

#define PRU0_MCASP0_TS0_POLL_IDX        0
#define PRU0_MCASP0_TS1_POLL_IDX        1
#define PRU1_MCASP1_TS0_POLL_IDX        2
#define PRU1_MCASP1_TS1_POLL_IDX        3

#define EP_TO_DEV_POLL_IDX              4
#define EP_TO_HST_POLL_IDX              5

#define POLL_TIMEOUT                    20

int main()
{
#if (C_STYLE)
    /*
     * First thread service ep0,
     * second thread will be created after successful configuration
     * and will handle IO data communication - look USB_REQ_SET_CONFIGURATION.
     */
    int ep0_fd = -1;
    if (cfg_ep0(&ep0_fd)) goto failure;
    handle_ep0(ep0_fd); /* Contains while(1) inside */
failure:
    if (ep0_fd != -1) close(ep0_fd);
    exit(-EXIT_FAILURE);
#else
    int                                     ret_val                 =-1;
    int                                     poll_fds_ammnt          = 0;
    double                                  pstart_time_stamp       = 0.0;
    struct pollfd                           fds        [POLL_FDS_AMMNT];
#if (RESAMPLE_8k_to_16k)
    int32_t resampled_buf  [sizeof(pckg_buf) * (sizeof(int32_t)/sizeof(int8_t)) * (16000/8000)];
#endif
    while (true)
    {
        poll_fds_ammnt = 0;
        bzero(fds, sizeof(fds));

        fds[poll_fds_ammnt].fd = pru0_mcasp0_ts0.fd;
        fds[poll_fds_ammnt].events = POLLOUT ;
        fds[poll_fds_ammnt].revents = 0;
        poll_fds_ammnt++;

        fds[poll_fds_ammnt].fd = pru0_mcasp0_ts1.fd;
        fds[poll_fds_ammnt].events = POLLOUT ;
        fds[poll_fds_ammnt].revents = 0;
        poll_fds_ammnt++;

        fds[poll_fds_ammnt].fd = pru1_mcasp1_ts0.fd;
        fds[poll_fds_ammnt].events = POLLOUT ;
        fds[poll_fds_ammnt].revents = 0;
        poll_fds_ammnt++;

        fds[poll_fds_ammnt].fd = pru1_mcasp1_ts1.fd;
        fds[poll_fds_ammnt].events = POLLOUT ;
        fds[poll_fds_ammnt].revents = 0;
        poll_fds_ammnt++;

        pstart_time_stamp = Get_timestamp_ms();
        ret_val = poll  (
                        fds,            /* The set of file descriptors to be monitored. */
                        poll_fds_ammnt, /* The number of items in the array of 'struct pollfd' type. */
                        POLL_TIMEOUT    /* The number of milliseconds that 'poll()' should block waiting for a file descriptor to become ready. */
                        );
        if (ret_val == -1) { FPRINTF; exit(-EXIT_FAILURE); }
        if (!ret_val)
        {
            PRINT_ERR("Timeout!. Elapsed %f ms.\r\n", (Get_timestamp_ms() - pstart_time_stamp));
            continue;
        }

        /* Send by chunks of 4 ms at a time */
        if (fds[0].revents & POLLOUT)
        {
            pru0_mcasp0_ts0.write_sin();
        }
        if (fds[1].revents & POLLOUT)
        {
            pru0_mcasp0_ts1.write_sin();
        }
        if (fds[2].revents & POLLOUT)
        {
            pru1_mcasp1_ts0.write_sin();
        }
        if (fds[3].revents & POLLOUT)
        {
            pru1_mcasp1_ts1.write_sin();
        }
    } //end while (1)
#endif /* C_STYLE */
}
