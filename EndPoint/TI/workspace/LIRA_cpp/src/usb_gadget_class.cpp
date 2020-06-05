#include "usb_gadget_class.hpp"

/* This variables should be independent
 * from class creation/destruction. */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
DECLARE_OUT_STREAM(usb_to_usmplr_, 0,0,0);
DECLARE_OUT_STREAM(usb_to_usmplr_, 0,0,1);
DECLARE_OUT_STREAM(usb_to_usmplr_, 0,1,0);
DECLARE_OUT_STREAM(usb_to_usmplr_, 0,1,1);

/* One set for all input streams */
std::mutex              in_mtx;
std::condition_variable in_cond_var;
queue_type              in_que;

DECLARE_IN_STREAM(agc_usb_,       1,0,0, in_mtx, in_cond_var, in_que);
DECLARE_IN_STREAM(agc_usb_,       1,0,1, in_mtx, in_cond_var, in_que);
DECLARE_IN_STREAM(agc_usb_,       1,1,0, in_mtx, in_cond_var, in_que);
DECLARE_IN_STREAM(agc_usb_,       1,1,1, in_mtx, in_cond_var, in_que);
#pragma GCC diagnostic pop
std::vector < struct usb_stream > usb_streams_vector =
{
    stream_usb_to_usmplr_000,
    stream_usb_to_usmplr_001,
    stream_usb_to_usmplr_010,
    stream_usb_to_usmplr_011,
          stream_agc_usb_100,
          stream_agc_usb_101,
          stream_agc_usb_110,
          stream_agc_usb_111
};

/*!
 * @param recv_pckg_bufs_vec_ref     - previously allocated list of buffers for package reception
 * @param stream_vector              - queue of received package - output of the class
 * @param recv_sample_rate           - receive sample rate calculated inside class
 */
usb_gadget::usb_gadget  (
                        std::vector < void * >&         recv_pckg_bufs_vector,
                        std::vector < struct usb_stream >& usb_streams_vector
                        ):
    recv_pckg_bufs_vec_ref   (recv_pckg_bufs_vector),
    usb_streams_vec_ref      (usb_streams_vector)
{
    if (cfg_ep0() != 0) { goto failure; }
    stop_ep0_thread = false;
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
    stop_read_thread = true;
    PRINTF(RED, "Close read-write endpoints.\n");
    if (to_dev_ep_fd >= 0) { close (to_dev_ep_fd); to_dev_ep_fd = -1; }
    if (to_hst_ep_fd >= 0) { close (to_hst_ep_fd); to_hst_ep_fd = -1; }
    PRINTF(RED, "Close ep0.\n");
    stop_ep0_thread = true;
    if (ep0_fd >= 0) { close (ep0_fd); ep0_fd = -1; }
    /* Give it time to finish read if it is in process */
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    /* Read call to usb gadget is blocking - it can blocks forever */
    read_thread.detach();
    ep0_thread.detach();
    PRINTF(RED, "USB gadget class destroyed.\n");
}
/*----------------------------------------------------------------------------*/

/*
 * Memory facility should be external to this class
 * to prevent program disrupt in case of class destruction.
 */
void usb_gadget::alloc_recv_pckg_buf(std::vector < void * >& recv_pckg_bufs_vec_ref)
{
    for(int i = 0; i < USB_IN_PCKG_BUF_AMMNT; i++)
    {
        uint8_t * pckg_buf = (uint8_t * )malloc(PAGE_SIZE);
        TEST_MALLOC_RES_N_PANISH(pckg_buf);
        recv_pckg_bufs_vec_ref.push_back(pckg_buf);
    }
}

void usb_gadget::dealloc_recv_pckg_buf(std::vector < void * >& recv_pckg_bufs_vec_ref)
{
    for(auto iter : recv_pckg_bufs_vec_ref)
    {
        FREE_MEM(iter);
    }
    recv_pckg_bufs_vec_ref.clear();
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

    /*! Configure ep2 toward device (read) */
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

    /*! Configure ep1 toward host (write) */
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

/*!
 * @param data              - pointer to the received
 * @param data_byte_length  - data bytes length exclude CRC bytes
 * @return                  - calculated CRC
 */
uint16_t usb_gadget::check_sum_calc(
        uint8_t * data,
        uint32_t data_byte_length
        ) {
    uint16_t crc_calc = 0xAAAA;
    uint16_t package_word_length = 0;
    uint16_t package_word = 0;          // Container for assembled words
    uint16_t data_byte_counter = 0;     // Counter for rendered data bytes

    /* Calculate data words in package */
    package_word_length = data_byte_length / 2;
    if (data_byte_length % 2 == 1) { /* If data quantity in package is odd */
        package_word_length++;
    }
    /* Calculate CRC */
    for (uint16_t counter = 0; counter < package_word_length; counter++) {
        package_word = ((uint16_t) (*data)); // Settle lower bits of 'package_word'
        data++;
        data_byte_counter++;
        if (data_byte_counter < data_byte_length) { // If data count is odd do not settle last byte in 'package_word'
            package_word = (package_word |
                    (((uint16_t) (*data)) << 8)); // Settle upper bits of 'package_word'
            data++;
            data_byte_counter++;
        }
        crc_calc = crc_calc ^ package_word;
        crc_calc = ((crc_calc << 1) & 0xFFFE) | ((crc_calc >> 15) & 0x0001);
    }
#if DEBUG_LI_GW
    PRINTF(MAG,"Calculated CRC: %x.\r\n", crc_calc);
#endif
    return crc_calc;
}

#define LI_GW_HDR_LNGTH         14
#define LI_GW_CRC_LNGTH         2
#define LI_GW_PCKG              true
#define LI_GW_ZERO_LENGTH       (LI_GW_HDR_LNGTH + LI_GW_CRC_LNGTH)

#define LI_GW_MSG_ID_ALAW_DATA             0x2000
#define LI_GW_FLAG_GET_STREAM_IDX(flag)    ((flag>>12)&0b11)

/*!
 * @param pckg_ptr   - pointer to the start of the received package
 * @param pckg_size  - input/output parameter - size of the received package/size of audio data
 * @param stream_idx - return parameter - index of output stream to use
 * @param adata      - return parameter - pointer at the start of the audiodata
 * @return
 */
void usb_gadget::de_LI_GW(  uint8_t *   pckg_ptr,
                            size_t&     pckg_size,
                            uint8_t&    stream_idx,
                            uint8_t **  adata )
{
    * adata = nullptr;
    stream_idx = 0x00;
    uint16_t recv_crc = 0;
    uint16_t cacl_crc = 0xAAAA;
    struct LI_GW_header header;
    if (pckg_size <= LI_GW_ZERO_LENGTH)
    {
        PRINT_ERR("Bad package length.\n");
        return;
    }
    recv_crc = *((uint16_t *) &pckg_ptr[pckg_size - LI_GW_CRC_LNGTH]);
    cacl_crc = check_sum_calc(pckg_ptr, pckg_size - LI_GW_CRC_LNGTH);
    if(recv_crc != cacl_crc)
    {
        PRINT_ERR(  "Wrong package CRC. In package: 0x%x. Calculated: 0x%x\r\n",
                    recv_crc, cacl_crc);
        return;
    }
    memcpy(&header, pckg_ptr, sizeof(struct LI_GW_header));
    * adata    = pckg_ptr + LI_GW_HDR_LNGTH;
    pckg_size -= LI_GW_ZERO_LENGTH;
    stream_idx = LI_GW_FLAG_GET_STREAM_IDX(header.msgFlags);
    if (header.msgID != LI_GW_MSG_ID_ALAW_DATA)
    {
        PRINT_ERR("Wrong package type.\n");
    }
    return;
}
/*----------------------------------------------------------------------------*/

void usb_gadget::read_thread_func(void)
{
    int ret_val = -1;
    auto pckg_buf_iter     = recv_pckg_bufs_vec_ref.begin();
    uint8_t * data_ptr = nullptr;
    uint8_t stream_idx = 0;
    size_t pckg_size   = 0;
    if (this->to_dev_ep_fd < 0) { PRINT_ERR("File not opened : fd=%i.\r\n", this->to_dev_ep_fd); return; }
#if (0)
    fd_set read_set;
    struct timeval timeout;
    timeout.tv_sec  = 0;                            /* Seconds */
    timeout.tv_usec = (__suseconds_t)(10 * 1000);   /* Microseconds */
#endif
    PRINTF(GRN, "Read thread started.\n");
    while (!stop_read_thread)
    {
    #if (0)
        FD_ZERO(&read_set);
        FD_SET(to_dev_ep_fd, &read_set);
        ret_val = select(
                        to_dev_ep_fd + 1,
                        &read_set,
                        N,
                        NULL,
                        &timeout
                        );
        if (ret_val == 0) { continue; } /* Timeout */
        if (ret_val < 0) { FPRINTF; continue; }
    #endif
        bzero(*pckg_buf_iter, PAGE_SIZE);
        ret_val = read (to_dev_ep_fd, * pckg_buf_iter, PAGE_SIZE);
        if (ret_val < 0) { FPRINTF; continue; }
#if (LI_GW_PCKG)
        pckg_size = ret_val;
        de_LI_GW((uint8_t *)*pckg_buf_iter, pckg_size, stream_idx, &data_ptr);
        if(data_ptr == nullptr) { continue; }
#else
        void * data_ptr = * pckg_buf_iter;
#endif
#if (REPACK_TO_4ms_CHUNKS)
        /* Re-pack received package in FIFO of 4ms data chunks. */
        uint32_t    pckg_buf_cntr  = 0;
        while ((int(pckg_buf_cntr) < ret_val)
                 &&(pckg_buf_cntr  < PAGE_SIZE))
        {
#endif
        struct usb_stream& usb_stream_ref = usb_streams_vector.at( stream_idx );
        {
            std::lock_guard<std::mutex> lock(* usb_stream_ref.mtx_ptr);
            usb_stream_ref.que.emplace((void *)data_ptr, pckg_size); /* Insert element at the end */
            if (usb_stream_ref.que.size() > MAX_USB_OUT_QUE_SIZE)
            {
                PRINT_ERR("No consumer. Index : 0x%x. Size : %d.\n",
                          stream_idx, usb_stream_ref.que.size());
                while (usb_stream_ref.que.size() > 2)
                    usb_stream_ref.que.pop();
            }
        } //end lock_guard
        usb_stream_ref.cond_var_ptr->notify_one();
#if (REPACK_TO_4ms_CHUNKS)
            pckg_buf_cntr += IN_BUF_SIZE_4ms_BYTES;
        }//end while
#endif
        /* Set next recv. buffer in use. */
        pckg_buf_iter++;
        if (pckg_buf_iter == recv_pckg_bufs_vec_ref.end())
        { pckg_buf_iter = recv_pckg_bufs_vec_ref.begin(); }
#if (0)
        this->gather_recv_stat(pckg_size, usb_stream_ref.sample_rate);
#else
        this->gather_recv_stat(pckg_size, usb_stream_ref.r_stat);
#endif
    } //end while (!stop_read_thread)
    PRINTF(RED, "Read thread finished.\n");
    return;
}
/*----------------------------------------------------------------------------*/
#if (0)
void usb_gadget::gather_recv_stat(size_t data_bytes, double& recv_sample_rate)
{
    double period     = 0.0;
    if (!prev_tspec.tv_sec)
    {
        clock_gettime(CLOCK_REALTIME, &prev_tspec);
        return;
    }
    recv_bytes_sum += data_bytes;
    recv_pckgs_counter++;
    if(recv_pckgs_counter == USB_IN_PCKGS_PER_SECOND)
    {
        /* save to stat */
        struct timespec curr_tspec;
        clock_gettime(CLOCK_REALTIME, &curr_tspec);
        period = timespecs_to_delta (
                                    prev_tspec,
                                    curr_tspec
                                    );
        recv_stat_que.emplace (period, recv_bytes_sum);
        recv_stat_period    += period;
        recv_bytes_mov_avrg += recv_bytes_sum;

        /* Clean up */
        recv_bytes_sum = 0;
        recv_pckgs_counter = 0;
        memcpy(&prev_tspec, &curr_tspec, sizeof(struct timespec));
    }
    else return;
    if(recv_stat_que.size() > RD_STAT_MAX_SIZE)
    {
        recv_stat_period    -= recv_stat_que.front().first;
        recv_bytes_mov_avrg -= recv_stat_que.front().second;
        recv_byte_rate       = double(recv_bytes_mov_avrg)/(recv_stat_period / 1.0e6);
        recv_sample_rate     = recv_byte_rate;
#if (SHOW_RECV_STAT)
        printf( CYN
                "USB : Pckgs. : %d units. "
                "Data : %d bytes. "
                "Period : %d microsecs. "
                "Data rate : %d bytes/sec.\n" NORM,
                RD_STAT_MAX_SIZE * USB_IN_PCKGS_PER_SECOND,
                recv_bytes_mov_avrg,
                int(recv_stat_period),
                int(recv_byte_rate)
                );
#endif
        recv_stat_que.pop();
    }
    else return;
}
/*----------------------------------------------------------------------------*/
#else
void usb_gadget::gather_recv_stat(size_t data_bytes, struct recv_stat& r_stat)
{
    double period     = 0.0;
    if (!r_stat.prev_tspec.tv_sec)
    {
        clock_gettime(CLOCK_REALTIME, &r_stat.prev_tspec);
        return;
    }
    r_stat.recv_bytes_sum += data_bytes;
    r_stat.recv_pckgs_counter++;
    if(r_stat.recv_pckgs_counter == USB_IN_PCKGS_PER_SECOND)
    {
        /* save to stat */
        struct timespec curr_tspec;
        clock_gettime(CLOCK_REALTIME, &curr_tspec);
        period = timespecs_to_delta (
                                    r_stat.prev_tspec,
                                    curr_tspec
                                    );
        r_stat.recv_stat_que.emplace (period, r_stat.recv_bytes_sum);
        r_stat.recv_stat_period    += period;
        r_stat.recv_bytes_mov_avrg += r_stat.recv_bytes_sum;

        /* Clean up */
        r_stat.recv_bytes_sum = 0;
        r_stat.recv_pckgs_counter = 0;
        memcpy(&r_stat.prev_tspec, &curr_tspec, sizeof(struct timespec));
    }
    else return;
    if(r_stat.recv_stat_que.size() > RD_STAT_MAX_SIZE)
    {
        r_stat.recv_stat_period    -= r_stat.recv_stat_que.front().first;
        r_stat.recv_bytes_mov_avrg -= r_stat.recv_stat_que.front().second;
        r_stat.recv_byte_rate       = double(r_stat.recv_bytes_mov_avrg)
                                           /(r_stat.recv_stat_period / 1.0e6);
        r_stat.recv_sample_rate     = r_stat.recv_byte_rate; /* 1byte == 1sample for alaw */
#if (SHOW_RECV_STAT)
        printf( CYN
                "USB : Pckgs. : %d units. "
                "Data : %d bytes. "
                "Period : %d microsecs. "
                "Data rate : %d bytes/sec.\n" NORM,
                RD_STAT_MAX_SIZE * USB_IN_PCKGS_PER_SECOND,
                r_stat.recv_bytes_mov_avrg,
                int(r_stat.recv_stat_period),
                int(r_stat.recv_byte_rate)
                );
#endif
        r_stat.recv_stat_que.pop();
    }
    else return;
}
/*----------------------------------------------------------------------------*/
#endif

inline void usb_gadget::write_block_mechanism(struct usb_stream& usb_stream_ref)
{
    while (!stop_write_thread)
    {
        std::unique_lock<std::mutex> uni_lock(* usb_stream_ref.mtx_ptr);
        if(usb_stream_ref.cond_var_ptr->wait_for(
                                    uni_lock,
                                    std::chrono::milliseconds(4),
                                    [&]{ return  !usb_stream_ref.que.empty(); }
                                    )!=true)
        {
            continue;
        }
        else
        {
            break;
        }
    }
}
/*----------------------------------------------------------------------------*/

void usb_gadget::write_thread_func(void)
{
    int ret_val = -1;
    uint8_t * buf_ptr  = nullptr;
    size_t    buf_size = 0;
    /* All input streams points to the same mutex, condition variable and queue */
    struct usb_stream& usb_stream_ref = usb_streams_vector.at( TO_STREAM_IDX(DIR_TO_USB,MCASP0,TS0) );
    PRINTF(GRN, "Write thread started.\n");
    while (!stop_read_thread)
    {
        write_block_mechanism(usb_stream_ref);
        {
            std::lock_guard<std::mutex> lock(* usb_stream_ref.mtx_ptr);
            buf_ptr  = (uint8_t *)usb_stream_ref.que.front().first;
            buf_size =            usb_stream_ref.que.front().second;
        }
        if((buf_ptr  == nullptr)
         ||(buf_size == 0))
        {
            PRINT_ERR("Empty queue.\n");
            std::lock_guard<std::mutex> lock(* usb_stream_ref.mtx_ptr);
            usb_stream_ref.que.pop();
            continue;
        }
        ret_val = write (to_hst_ep_fd, buf_ptr, buf_size);
        if (ret_val != int(buf_size)) { FPRINTF; }
        {
            std::lock_guard<std::mutex> lock(* usb_stream_ref.mtx_ptr);
            usb_stream_ref.que.pop();
        } //end lock_guard


    } //end while
}
/*----------------------------------------------------------------------------*/

