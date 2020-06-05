#include "usb_gadget_class.hpp"

usb_gadget::usb_gadget(void):
ep0_fd(-1), to_hst_ep_fd(-1), to_dev_ep_fd(-1)
{
    if (cfg_ep0() != 0) { goto failure; }
    stop_ep0_thrd = 0;

    ep0_thread = std::thread(&usb_gadget::handle_ep0_thread_func, this);
    return;
failure:
    if (ep0_fd != -1) { close(ep0_fd); }
    exit(-EXIT_FAILURE);
}
/*----------------------------------------------------------------------------*/

usb_gadget::~usb_gadget(void)
{
    PRINTF(RED, "Stopping read thread.\n");
    stop_read_thrd = 1;
    PRINTF(RED, "Close read-write endpoints.\n");
    if (to_dev_ep_fd >= 0) { close (to_dev_ep_fd); to_dev_ep_fd = -1; }
    if (to_hst_ep_fd >= 0) { close (to_hst_ep_fd); to_hst_ep_fd = -1; }

    read_thread.join();


    PRINTF(RED, "Close ep0.\n");
    stop_ep0_thrd = 1;
    if (ep0_fd >= 0) { close (ep0_fd); ep0_fd = -1; }
    ep0_thread.join();
}
/*----------------------------------------------------------------------------*/

int usb_gadget::cfg_rd_wr_ep(void)
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
    if (to_dev_ep_fd !=-1 )
    {
        PRINT_ERR("%s already configured.\n", usb_to_dev_ep_dev_name.c_str());
        goto exit;
    }
    /* Open dev file */
    to_dev_ep_fd = open(usb_to_dev_ep_dev_name.c_str(), O_RDWR );
    if (to_dev_ep_fd <= 0) { FPRINTF_EXT("Unable to open %s\n", usb_to_dev_ep_dev_name.c_str()); goto exit; }

    /* Prepare config */
    tag = 0x00000001;
    send_size = 0;
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

    /* Write config */
    ret_val = write(to_dev_ep_fd, init_config, send_size);
    if (ret_val != (int)send_size)
    {
        FPRINTF_EXT("Can't write %s(%i).\n", usb_to_dev_ep_dev_name.c_str(), to_dev_ep_fd);
        goto exit;
    }
    PRINTF(GRN, "%s is configured\n", usb_to_dev_ep_dev_name.c_str());
#endif

    /*!
     * Configure ep1 toward host (write)
     */
#if (1)
    if (to_hst_ep_fd != -1)
    {
        PRINT_ERR("%s already configured.\n", usb_to_hst_ep_dev_name.c_str());
        goto exit;
    }

    /* Open dev file */
    to_hst_ep_fd = open(usb_to_hst_ep_dev_name.c_str(), O_RDWR );
    if (to_hst_ep_fd <= 0)
    {
        PRINT_ERR("Unable to open %s (%m)\n", usb_to_hst_ep_dev_name.c_str());
        goto exit;
    }

    /* Prepare config */
    tag = 0x00000001;
    send_size = 0;
    cfg_ptr = &init_config[0];
    memcpy(cfg_ptr, &tag, sizeof(tag));
    cfg_ptr += sizeof(tag);
    send_size += sizeof(tag);
    memcpy(cfg_ptr, &to_hst_ep_descriptor, to_hst_ep_descriptor.bLength);
    cfg_ptr += to_hst_ep_descriptor.bLength;
    send_size += to_hst_ep_descriptor.bLength;
    memcpy(cfg_ptr, &to_hst_ep_descriptor, to_hst_ep_descriptor.bLength);
    cfg_ptr += to_hst_ep_descriptor.bLength;
    send_size += to_hst_ep_descriptor.bLength;

    /* Write config */
    ret_val = write(to_hst_ep_fd, init_config, send_size);
    if (ret_val != (int)send_size)
    {
        FPRINTF_EXT("Can't write %s.\n", usb_to_hst_ep_dev_name.c_str());
        goto exit;
    }
    PRINTF(GRN, "%s is configured\n", usb_to_hst_ep_dev_name.c_str());
#endif



    ret_val = 0;
    exit: return ret_val;
}
/*----------------------------------------------------------------------------*/

void usb_gadget::handle_setup_request(struct usb_ctrlrequest * setup_ptr)
{
    int status;
    uint8_t buffer[MAX_USB_PCKG_SIZE];

    PRINTF(YEL, "Handling setup request.\n");
    switch (setup_ptr->bRequest)
    {
        case USB_REQ_GET_DESCRIPTOR:
        {
        #if (1)
            PRINTF(BLU,"GET_DESCRIPTOR (%d) request.\n", setup_ptr->bRequest);
            if (setup_ptr->bRequestType != USB_DIR_IN)
            {
                PRINT_ERR("Bad direction : 0x%x.\n", setup_ptr->bRequestType);
                goto stall;
            }
            switch (setup_ptr->wValue >> 8)
            {
                case USB_DT_STRING:
                {
                    PRINTF(CYN, "Request STRING (0x%x). Descriptor index : %d (max length %d)\n",
                           (setup_ptr->wValue >> 8), setup_ptr->wValue & 0xff, setup_ptr->wLength);
                    {
                        extern int usb_gadget_get_string (struct usb_gadget_strings *table, int id, __u8 *buf);
                        status = usb_gadget_get_string (&strings, setup_ptr->wValue & 0xff, buffer);
                    }
                    if (status < 0) /* Error */
                    {
                        PRINT_ERR("String not found.\n");
                        break;
                    }
                    else
                    {
                        PRINTF(GRN, "Found string #%d. bLength = %d.\n", setup_ptr->wValue, status);
                    }
                    write (ep0_fd, buffer, status);
                    return;
                } //end case USB_DT_STRING
                default: PRINT_ERR("Unknown descriptor type request: %d\n", (setup_ptr->wValue >> 8));
            } //end switch (setup->wValue >> 8)
            break;
        #endif
        } //end USB_REQ_GET_DESCRIPTOR
        case USB_REQ_SET_CONFIGURATION:
        {
        #if (1)
            PRINTF(BLU,"SET_CONFIGURATION (%d) request.\n", setup_ptr->bRequest);
            if (setup_ptr->bRequestType != USB_DIR_OUT) { PRINT_ERR("Bad direction.\n"); goto stall; }
            switch (setup_ptr->wValue)
            {
                case CONFIG_VALUE:
                {
                    PRINTF(CYN, "Initializing endpoints.\n");
                    status = cfg_rd_wr_ep();
                    stop_read_thrd = 0;
                    read_thread = std::thread(&usb_gadget::read_thread_func, this);
                    break;
                } //end case CONFIG_VALUE
                case 0:
                {
                    stop_read_thrd = 1;
                    read_thread.join();
                    PRINTF(RED, "Close read-write endpoints.\n");
                    if (to_dev_ep_fd >= 0) { close (to_dev_ep_fd); to_dev_ep_fd = -1; }
                    if (to_hst_ep_fd >= 0) { close (to_hst_ep_fd); to_hst_ep_fd = -1; }
                    break;
                }
                default:
                {
                    PRINT_ERR("Un-handled configuration value %d\n", setup_ptr->wValue);
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
            ioctl (to_dev_ep_fd, GADGETFS_CLEAR_HALT);
            ioctl (to_hst_ep_fd, GADGETFS_CLEAR_HALT);
            // ACK
            status = read (ep0_fd, &status, 0);
            return;
        #endif
        } //end USB_REQ_SET_INTERFACE
        default : { PRINT_ERR("Un-handled setup request %d.\n", setup_ptr->bRequest); }
    } //end switch

stall:
    PRINT_ERR("Stalled\n");
    // Error
    if (setup_ptr->bRequestType & USB_DIR_IN)   { read  (ep0_fd, &status, 0); }
    else                                        { write (ep0_fd, &status, 0); }
}
/*----------------------------------------------------------------------------*/

void usb_gadget::handle_ep0_events  (
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
                PRINTF(CYN, "ep0 connect event.\n"); break;
            case GADGETFS_DISCONNECT:
                PRINTF(CYN, "ep0 disconnect event.\n"); break;
        #endif
            case GADGETFS_SETUP:
            {
                PRINTF(CYN, "ep0 setup event.\n");
                handle_setup_request(&event_ptr[i].u.setup);
                break;
            }
            case GADGETFS_NOP: /* Fall through */
            case GADGETFS_SUSPEND: break;
        } //end switch
    } //end for
}
/*----------------------------------------------------------------------------*/

#define NEVENT  5
void usb_gadget::handle_ep0_thread_func(void)
{
    int ret_val = -1;
    int events_ammnt;
    fd_set read_set;
    struct usb_gadgetfs_event events[NEVENT];
    struct timeval timeout;
    timeout.tv_sec  = 0;                            /* Seconds */
    timeout.tv_usec = (__suseconds_t)(10 * 1000);   /* Microseconds */
    while (!stop_ep0_thrd)
    {
        FD_ZERO(&read_set);
        FD_SET(ep0_fd, &read_set);
        ret_val = select(
                        ep0_fd + 1,
                        &read_set,
                        NULL,
                        NULL,
                        &timeout
                        );
        if (ret_val == 0) { continue; } /* Timeout */
        if (ret_val < 0) { FPRINTF; goto error; }

        ret_val = read(ep0_fd, &events, sizeof(events));
        if (ret_val < 0) { FPRINTF; goto error; }

        events_ammnt = ret_val / sizeof(events[0]);
        PRINTF(MAG, "%d event(s) happened.\n", events_ammnt);
        handle_ep0_events   (
                            &events[0],
                            events_ammnt
                            );
    } //end while (1)
error: return;
}
/*----------------------------------------------------------------------------*/

void usb_gadget::read_thread_func(void)
{
    int ret_val = -1;
    uint8_t * pckg_buf = NULL;
    while (!stop_read_thrd)
    {
        pckg_buf = (uint8_t *)malloc(PAGE_SIZE);
        if(!pckg_buf) { FPRINTF; exit(-EXIT_FAILURE); }
        bzero(pckg_buf, sizeof(PAGE_SIZE));
        ret_val = read (to_dev_ep_fd, (void *)pckg_buf, PAGE_SIZE);
        if (ret_val < 0)
        {
            FPRINTF;
            if(pckg_buf) { free(pckg_buf); pckg_buf = NULL; }
        }
        else
        {
            /* Process package */
            recv_vec.push(std::make_pair(pckg_buf, ret_val)); /* Insert element at the end. */
            /* Gather stat */
            struct timespec tspec;
            clock_gettime(CLOCK_REALTIME, &tspec);
            recv_stat_vec.emplace_back(tspec, ret_val);
            recv_bytes_mov_avrg += ret_val;
            if (recv_vec.size() > MAX_PCKGS_IN_QUEUE)
            {
                pckg_buf = recv_vec.front().first;
                if(pckg_buf)
                {
                    free(pckg_buf); pckg_buf = NULL;
                }
                recv_vec.pop();
            }

            if(recv_stat_vec.size() > RD_STAT_MAX_SIZE)
            {
                recv_bytes_mov_avrg -= recv_stat_vec.front().second;
#if (DEBUG_READ_STAT)
                double period = timespecs_to_delta  (
                                                    recv_stat_vec[0].first,
                                                    recv_stat_vec[RD_STAT_MAX_SIZE].first
                                                    );
                double rate = double(recv_bytes_mov_avrg) * 1000.0/period;
                printf( "Pckgs. : %d units. "
                        "Data : %d bytes. "
                        "Period : %d ms. "
                        "Data rate : %d bytes/sec.\n",
                        RD_STAT_MAX_SIZE,
                        recv_bytes_mov_avrg,
                        int(period),
                        int(rate)
                        );
#endif
                recv_stat_vec.erase(recv_stat_vec.begin());
            }
        } //end if (recv_bytes < 0)
    } //end while (!stop_read_thrd)

    /* Clean pckg. queue */
    while(recv_vec.size() > 0)
    {
        pckg_buf = recv_vec.front().first;
        if(pckg_buf)
        {
            free(pckg_buf); pckg_buf = NULL;
        }
        recv_vec.pop();
    }
    return;
}
/*----------------------------------------------------------------------------*/

int usb_gadget::handle_ep0(void)
{
    int events_ammnt;
    struct usb_gadgetfs_event events[NEVENT];
    int ret_val = read(ep0_fd, &events, sizeof(events));
    if (ret_val < 0) { FPRINTF; return ret_val; }

    events_ammnt = ret_val / sizeof(events[0]);
    PRINTF(MAG, "%d event(s) happened.\n", events_ammnt);
    handle_ep0_events   (
                        &events[0],
                        events_ammnt
                        );
    return 0;
}
/*----------------------------------------------------------------------------*/

int usb_gadget::cfg_ep0(void)
{
    int       ret_val           = -1;
    uint32_t  send_size         = 0;
    uint8_t   init_config[2048] = { 0 };
    uint8_t * cfg_ptr           = &init_config[0];
    /* Open dev file */
    ep0_fd = open(usb_dev_name.c_str(), O_RDWR | O_SYNC| S_IWUSR | S_IRUSR );
    if (ep0_fd <= 0)
    {
        FPRINTF_EXT("Unable to open %s.\n", usb_dev_name.c_str());
        exit(-EXIT_FAILURE);
    }
    /* Prepare config */
    uint32_t tag = 0x00000000;
    memcpy(cfg_ptr, &tag,                   sizeof(tag));                   cfg_ptr += sizeof(tag);
    memcpy(cfg_ptr, &config,                config.bLength);                cfg_ptr += config.bLength;
    memcpy(cfg_ptr, &interface_descriptor,  interface_descriptor.bLength);  cfg_ptr += interface_descriptor.bLength;
    memcpy(cfg_ptr, &to_dev_ep_descriptor,  to_dev_ep_descriptor.bLength);  cfg_ptr += to_dev_ep_descriptor.bLength;
    memcpy(cfg_ptr, &to_hst_ep_descriptor,  to_hst_ep_descriptor.bLength);  cfg_ptr += to_hst_ep_descriptor.bLength;


    memcpy(cfg_ptr, &config_hs,             config_hs.bLength);             cfg_ptr += config_hs.bLength;
    memcpy(cfg_ptr, &interface_descriptor,  interface_descriptor.bLength);  cfg_ptr += interface_descriptor.bLength;
    memcpy(cfg_ptr, &to_dev_ep_descriptor,  to_dev_ep_descriptor.bLength);  cfg_ptr += to_dev_ep_descriptor.bLength;
    memcpy(cfg_ptr, &to_hst_ep_descriptor,  to_hst_ep_descriptor.bLength);  cfg_ptr += to_hst_ep_descriptor.bLength;


    memcpy(cfg_ptr, &device_descriptor,     device_descriptor.bLength);     cfg_ptr += device_descriptor.bLength;
    send_size = (uint32_t)cfg_ptr-(uint32_t)init_config;
    /*
     * Configure ep0
     * A special endpoint called ep0 (the first one) is always present.
     * It's an in and out endpoint with control attribute.
     * It allows to read/write configuration of other endpoints.
     */
    ret_val = write(ep0_fd, init_config, send_size);
    if (ret_val != (int)send_size) { FPRINTF; return -1; }
    PRINTF(GRN, "ep0(%s) is configured\n", usb_dev_name.c_str());
    return 0;
}
/*----------------------------------------------------------------------------*/
