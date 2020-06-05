#include "mcasp_class.hpp"
/*                   PRU# MCASP# TS#*/
DECLARE_MCASP_DATA(PRU0, MCASP0, TS0);
DECLARE_MCASP_DATA(PRU0, MCASP0, TS1);
DECLARE_MCASP_DATA(PRU1, MCASP1, TS0);
DECLARE_MCASP_DATA(PRU1, MCASP1, TS1);

#define OPEN_STAT_FILE(file_name, ts_idx)                                                       \
    fname_##file_name << this->stat_dir.str() << "/" << #file_name << "_ts" << int(ts_idx + 1); \
    PRINTF(MAG, "Opening file '%s'\n", fname_##file_name .str().c_str());                       \
    this->fd_##file_name = open(fname_##file_name .str().c_str(), O_RDWR | S_IWUSR | S_IRUSR ); \
    if (this->fd_##file_name < 0)                                                               \
        { FPRINTF_EXT("Can't open file %s.\n", fname_##file_name .str().c_str()); }             \
    else                                                                                        \
        { PRINTF(GRN, "File %s successfully opened.\n", fname_##file_name .str().c_str()); }

/*!
 * @param recv_pckg_bufs_vector - previously allocated list of buffers for package reception
 * @param mcasp_data            - data for/about current MCASP class instance
 */
mcasp_ts::mcasp_ts(std::vector < void * >& recv_pckg_bufs_vector,
                   struct mcasp_data&      mcasp_data) :
    recv_pckg_bufs_vec_ref(recv_pckg_bufs_vector),
    mcasp_data_ref        (mcasp_data)
{
    this->pru_mcasp_fname << "/dev/"
                          << "mcasp" << std::to_string(mcasp_data_ref.mcasp_idx)
                          << "_pru"  << std::to_string(mcasp_data_ref.pru_idx)
                          << "_ts"   << std::to_string(mcasp_data_ref.ts_idx);
    PRINTF(MAG, "Opening file : %s.\n", pru_mcasp_fname.str().c_str());
    this->pru_mcasp_fd = open(pru_mcasp_fname.str().c_str(), O_RDWR | S_IWUSR | S_IRUSR );
    if (pru_mcasp_fd < 0) { FPRINTF_EXT("Can't open file '%s'.\r\n", pru_mcasp_fname.str().c_str()); goto failure; }
    prev_tspec.tv_nsec = 0;
    prev_tspec.tv_sec  = 0;
    this->stat_dir << std::hex
                   << "/sys/devices/platform/ocp"
                   << "/" << PRUSS_SOC_BUS_ADDD << ".pruss-soc-bus"
                   << "/" << PRUSS_ADDR         << ".pruss"
                   << "/" << int(mcasp_data_ref.pru_idx ? PRU_1_SOC_BUS_ADDR : PRU_0_SOC_BUS_ADDR)
                          << ".pru" << int(mcasp_data_ref.pru_idx)
                   << "/remoteproc"
                   << "/remoteproc" << int(mcasp_data_ref.pru_idx + 1)
                   << "/virtio"     << int(mcasp_data_ref.pru_idx)
                   << "/virtio"     << int(mcasp_data_ref.pru_idx)
                   << ".mcasp"      << int(mcasp_data_ref.mcasp_idx)
                   << "_pru"        << int(mcasp_data_ref.pru_idx)
                   << ".-1.3"       << int(mcasp_data_ref.pru_idx)
                   << "/pru_mcasp";
    OPEN_STAT_FILE(last_tx_msg, mcasp_data_ref.ts_idx);
    OPEN_STAT_FILE(last_rx_msg, mcasp_data_ref.ts_idx);
    return;

failure:
    delete this;
    return;
}
/*----------------------------------------------------------------------------*/

mcasp_ts::~mcasp_ts(void)
{
    if (this->pru_mcasp_fd >= 0)
    {
        close(this->pru_mcasp_fd);
    }
    this->stop();
    PRINTF(RED, "PRU%d MCASP%d TS%d is destroyed.\n",
           mcasp_data_ref.pru_idx,
           mcasp_data_ref.mcasp_idx,
           mcasp_data_ref.ts_idx);
}
/*----------------------------------------------------------------------------*/

/*
 * Memory facility should be external to this class
 * to prevent program disrupt in case of class destruction.
 */
void mcasp_ts::alloc_recv_pckg_buf(std::vector < void * >& recv_pckg_bufs_vec_ref)
{
    for(int i = 0; i < MCASP_IN_PCKG_BUF_AMMNT; i++)
    {
        uint8_t * pckg_buf = (uint8_t * )malloc(PAGE_SIZE);
        TEST_MALLOC_RES_N_PANISH(pckg_buf);
        recv_pckg_bufs_vec_ref.push_back(pckg_buf);
    }
}

void mcasp_ts::dealloc_recv_pckg_buf(std::vector < void * >& recv_pckg_bufs_vec_ref)
{
    for(auto iter : recv_pckg_bufs_vec_ref)
    {
        FREE_MEM(iter);
    }
    recv_pckg_bufs_vec_ref.clear();
}
/*----------------------------------------------------------------------------*/

void mcasp_ts::write_sin(void)
{
    #define SIN_FREQ                2000.0
    #define FS_FREQ                 16000.0
    #define SIN_PERIOD              (double)(FS_FREQ / SIN_FREQ)
    int32_t ret_val;
    int32_t send_buf[BUF_SIZE_4ms_WORDS] = {0};
    if (this->pru_mcasp_fd < 0) goto failure;

    for ( int i = 0; i < int(BUF_SIZE_4ms_WORDS); i++ )
    {
        send_buf[i] = (int32_t)(20000.0 * (double) sin ( (i / SIN_PERIOD) * 2 * M_PI ) );
    }
    ret_val = write(
                    this->pru_mcasp_fd,
                    send_buf,
                    BUF_SIZE_4ms_BYTES
                    );
    if (ret_val < 0) FPRINTF;
    return;
failure:
    PRINT_ERR("File not opened : fd=%i.\r\n", this->pru_mcasp_fd);
    return;
}
/*----------------------------------------------------------------------------*/

#define TEST_FILE(fd) \
    if (fd < 0) { PRINT_ERR("File not opened : fd=%i.\r\n", fd); return; }

void mcasp_ts::write_mcasp(void)
{
    int32_t ret_val;
    void * send_buf_ptr = nullptr;
    size_t send_buf_size = 0;
    TEST_FILE(this->pru_mcasp_fd);
    {
        std::lock_guard<std::mutex> lock(* mcasp_data_ref.in_mtx_ptr);
        shrink_in_que(); /* Shrink send queue if it is too long to reduce delay */
        if (mcasp_data_ref.in_que.empty()) { PRINT_ERR("Empty queue.\n"); return; }
        send_buf_ptr  = mcasp_data_ref.in_que.front().first;
        send_buf_size = mcasp_data_ref.in_que.front().second;
    }
    if ((send_buf_ptr==nullptr)||(!send_buf_size))
    {
        PRINT_ERR("Buffer is NULL.\n");
        goto exit;
    }
    ret_val = write (
                    this->pru_mcasp_fd,
                    send_buf_ptr,
                    send_buf_size
                    );
    if (ret_val < 0) { FPRINTF; }
    else             { this->gather_send_stat(ret_val); }

exit:
    {
        std::lock_guard<std::mutex> lock(* mcasp_data_ref.in_mtx_ptr);
        mcasp_data_ref.in_que.pop(); /* Remove first element */
    }
    return;
}
/*----------------------------------------------------------------------------*/

void mcasp_ts::gather_send_stat(size_t sent_bytes)
{
    double period     = 0.0;
    if (!prev_tspec.tv_sec)
    {
        clock_gettime(CLOCK_REALTIME, &prev_tspec);
        return;
    }
    send_bytes_sum += sent_bytes;
    send_pckgs_counter++;
    if(send_pckgs_counter == MCASP_OUT_PCKGS_PER_SECOND)
    {
        /* save to stat */
        struct timespec curr_tspec;
        clock_gettime(CLOCK_REALTIME, &curr_tspec);
        period = timespecs_to_delta (
                                    prev_tspec,
                                    curr_tspec
                                    );
        send_stat_que.emplace (period, send_bytes_sum);
        send_stat_period    += period;
        send_bytes_mov_avrg += send_bytes_sum;

        /* Clean up */
        send_bytes_sum     = 0;
        send_pckgs_counter = 0;
        memcpy(&prev_tspec, &curr_tspec, sizeof(struct timespec));
    }
    else return;
    if(send_stat_que.size() > WR_STAT_MAX_SIZE)
    {
        send_stat_period    -= send_stat_que.front().first;
        send_bytes_mov_avrg -= send_stat_que.front().second;
        write_byterate         = double(send_bytes_mov_avrg)/(send_stat_period / 1.0e6);
#if (SHOW_STAT)
        printf( MAG "mcast%d ts%d : "
                "Pckgs. : %d units. "
                "Data : %d bytes. "
                "Period : %d microsecs. "
                "Data rate : %d bytes/sec.\n" NORM,
                mcasp_idx, ts_idx,
                WR_STAT_MAX_SIZE * MCASP_OUT_PCKGS_PER_SECOND,
                send_bytes_mov_avrg,
                int(send_stat_period),
                int(write_byterate)
                );
#endif
        send_stat_que.pop();
        this -> gather_send_stat();
    }
    else return;
}
/*----------------------------------------------------------------------------*/

void mcasp_ts::gather_send_stat ()
{
    int32_t ret_val = -1;
    char buf[64];
    if (this->fd_last_tx_msg < 0) { PRINT_ERR("File not opened : fd=%i.\r\n", this->fd_last_tx_msg); return; }
    if (this->fd_last_rx_msg < 0) { PRINT_ERR("File not opened : fd=%i.\r\n", this->fd_last_rx_msg); return; }
    bzero(buf, sizeof(buf));
    ret_val = read  (
                    this->fd_last_tx_msg,
                    buf,
                    sizeof(buf)
                    );
    lseek(this->fd_last_tx_msg, 0, SEEK_SET);
    if (ret_val <= 0) { FPRINTF; }
    last_tx_msg_str       = std::string(&buf[0], strlen(buf));
    last_tx_msg_time_str  = last_tx_msg_str.substr(0, last_tx_msg_str.find(std::string(":")));
    last_tx_msg_num_str   = last_tx_msg_str.substr(last_tx_msg_str.find(std::string(":"))+1);
    try
    {
        last_tx_msg_t_off          = std::stod(last_tx_msg_time_str);
        last_tx_msg_num            = std::stoi(last_tx_msg_num_str);
        send_byte_rate             = (double(last_tx_msg_num) * BUF_SIZE_4ms_BYTES)/(last_tx_msg_t_off);
        mcasp_data_ref.sample_rate = (double(last_tx_msg_num) * BUF_SIZE_4ms_WORDS)/(last_tx_msg_t_off);
#if (SHOW_STAT)
        printf(BLU "mcast%d ts%d : Internal byterate : %f bytes/sec.\n",
                mcasp_idx, ts_idx, send_byte_rate);
#endif
    }
    catch (const std::exception& e)
    {
        PRINT_ERR( "%s\n", e.what());
    }
}
/*----------------------------------------------------------------------------*/

void mcasp_ts::start(void)
{
    stop_write_thread = false;
    write_thread = std::thread(&mcasp_ts::write_thread_func, this);
    stop_read_thread = false;
    read_thread = std::thread(&mcasp_ts::read_thread_func, this);
}

void mcasp_ts::stop(void)
{
    if (!stop_write_thread)
    {
        PRINTF(RED, "Stopping write thread.\n");
        stop_write_thread = true;
        write_thread.join();
    }
    else PRINT_ERR("Already stopped.\n");

    if (!stop_read_thread)
    {
        PRINTF(RED, "Stopping read thread.\n");
        stop_read_thread = true;
        read_thread.join();
    }
    else PRINT_ERR("Already stopped.\n");
}
/*----------------------------------------------------------------------------*/

inline void mcasp_ts::write_block_mechanism(void)
{
    while (!stop_write_thread)
    {
        std::unique_lock<std::mutex> uni_lock(* mcasp_data_ref.in_mtx_ptr);
        if(mcasp_data_ref.in_cond_var_ptr->wait_for(
                                    uni_lock,
                                    std::chrono::milliseconds(4),
                                    [&]{ return  !mcasp_data_ref.in_que.empty(); }
                                    )!=true)
        { continue; }
        else
        { break;    }
    } //end while
}

void mcasp_ts::write_thread_func(void)
{
    int           ret_val =-1;
    struct pollfd fds[1];
    TEST_FILE(this->pru_mcasp_fd);
    PRINTF(GRN, "Starting MCASP write thread.\n");
    while (!stop_write_thread)
    {
        write_block_mechanism();
        fds[0].fd      = this->pru_mcasp_fd;
        fds[0].events  = POLLOUT ;
        fds[0].revents = 0;
        ret_val = poll  (
                        fds,
                        1,
                        WRITE_POLL_TIMEOUT
                        );
        if (ret_val == -1) { FPRINTF; exit(-EXIT_FAILURE); }
        if (!ret_val)
        {
            PRINT_ERR("Timeout!\n");
            continue;
        }
        if (fds[0].revents & POLLOUT)
        {
            write_mcasp();
        }
    } //end while
}

void mcasp_ts::read_thread_func(void)
{
    int           ret_val =-1;
    struct pollfd fds[1];
    auto pckg_buf_iter = recv_pckg_bufs_vec_ref.begin();
    TEST_FILE(this->pru_mcasp_fd);
    PRINTF(GRN, "Starting MCASP read thread.\n");
    while (!stop_read_thread)
    {
        fds[0].fd      = this->pru_mcasp_fd;
        fds[0].events  = POLLIN ;
        fds[0].revents = 0;
        ret_val = poll  (
                        fds,
                        1,
                        READ_POLL_TIMEOUT
                        );
        if (ret_val == -1) { FPRINTF; exit(-EXIT_FAILURE); }
        if (!ret_val)
        {
            PRINT_ERR("Timeout!\n");
            continue;
        }
        if (fds[0].revents & POLLIN)
        {
            bzero(*pckg_buf_iter, PAGE_SIZE);
            ret_val = read(
                    this->pru_mcasp_fd,
                    * pckg_buf_iter,
                    MCASP_IN_BUF_SIZE_4ms_BYTES
                    );
            if (ret_val <= 0) { FPRINTF; continue; }
#if (0)
            printf("mcasp_idx : %d ts_idx : %d\n",
                    mcasp_data_ref.mcasp_idx, mcasp_data_ref.ts_idx);
#endif
            {
                std::lock_guard<std::mutex> lock(* mcasp_data_ref.out_mtx_ptr);
                mcasp_data_ref.out_que.emplace((void *)* pckg_buf_iter, ret_val); /* Insert element at the end */
                shrink_out_que();
            }
            mcasp_data_ref.out_cond_var_ptr->notify_one();
            pckg_buf_iter++;
            if (pckg_buf_iter == recv_pckg_bufs_vec_ref.end())
            { pckg_buf_iter = recv_pckg_bufs_vec_ref.begin(); }
        }
    } //end while
}
/*----------------------------------------------------------------------------*/

#define NORM_DELAY_IN_4ms_BUFS      5 /* One 20ms pckg it is a 5 pckgs by 4ms */
#define MAX_DELAY_IN_4ms_BUFS       (NORM_DELAY_IN_4ms_BUFS + 5)
inline void mcasp_ts::shrink_in_que(void)
{
    if (mcasp_data_ref.in_que.size() >= MAX_DELAY_IN_4ms_BUFS)
    {
#if (SHOW_IN_SHRINK)
        printf(RED"mcasp%dts%d in: Shrink. Size %d.\n" NORM,
                mcasp_data_ref.mcasp_idx, mcasp_data_ref.ts_idx,
                mcasp_data_ref.in_que.size());
#endif
        while (mcasp_data_ref.in_que.size() > NORM_DELAY_IN_4ms_BUFS) /* 24ms */
        {
            mcasp_data_ref.in_que.pop(); /* Remove first element */
        } //end while
    }
}
/*----------------------------------------------------------------------------*/

inline void mcasp_ts::shrink_out_que(void)
{
    if (mcasp_data_ref.out_que.size() >= MAX_DELAY_IN_4ms_BUFS)
    {
#if (SHOW_OUT_SHRINK)
        printf(RED"mcasp%dts%d out: Shrink. Size %d.\n" NORM,
                mcasp_data_ref.mcasp_idx, mcasp_data_ref.ts_idx,
                mcasp_data_ref.out_que.size());
#endif
        while (mcasp_data_ref.out_que.size() > NORM_DELAY_IN_4ms_BUFS) /* 24ms */
        {
            mcasp_data_ref.out_que.pop(); /* Remove first element */
        } //end while
    }
}
/*----------------------------------------------------------------------------*/
