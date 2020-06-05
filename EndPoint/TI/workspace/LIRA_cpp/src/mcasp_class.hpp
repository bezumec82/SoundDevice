#ifndef __MCASP_H__
#define __MCASP_H__

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <poll.h>

#include <iostream>
#include <thread>
#include <queue>
#include <sstream>
#include <mutex>
#include <condition_variable>

#include "debug.h"
#include "time_manip.hpp"
#ifndef PRINTF
#define PRINTF(color, text, ...)
#define PRINT_ERR(text, ...)
#define FPRINTF
#define FPRINTF_EXT(text, ...)
#endif
#define SHOW_STAT               false
#define SHOW_IN_SHRINK          true
#define SHOW_OUT_SHRINK          false

#define PAGE_SIZE               4096

#define BUF_SIZE_4ms_WORDS      64
#define BUF_SIZE_4ms_BYTES      (uint32_t)( BUF_SIZE_4ms_WORDS * sizeof (int32_t) )
#define BUF_SIZE_20ms_WORDS     (uint32_t)( BUF_SIZE_4ms_WORDS * 5 )
#define SEND_BUF_SIZE_WORDS     (uint32_t)( BUF_SIZE_4ms_WORDS )
#define SEND_BUF_SIZE_BYTES     (uint32_t)( SEND_BUF_SIZE_WORDS * sizeof (int32_t) )
#define RECV_BUF_SIZE_BYTES                 SEND_BUF_SIZE_BYTES



#define MCASP_IN_BUF_SIZE_4ms_WORDS     64 /* 16000Hz 16(32) bit   4ms == 4x16 = 64w */
#define MCASP_IN_BUF_SIZE_4ms_BYTES     ( MCASP_IN_BUF_SIZE_4ms_WORDS * sizeof (int32_t) )


#define SINE_TEST               false

#define PRU0                    0
#define PRU1                    1
#define MCASP0                  0
#define MCASP1                  1
#define TS0                     0
#define TS1                     1

#define DEBUG_MCASP_WRITE               false
#define DEBUG_MCASP_WRITE_STAT          true
#define MCASP_WR_RPT_LAST_SMPL          false

#define MCASP_PCKG_SIZE_MS              4
#define MCASP_OUT_PCKGS_PER_SECOND      (1000/MCASP_PCKG_SIZE_MS)
#define WR_STAT_MAX_SIZE                10

#define PRUSS_SOC_BUS_ADDD              0x4a326000
#define PRUSS_ADDR                      0x4a300000
#define PRU_0_SOC_BUS_ADDR              0x4a334000
#define PRU_1_SOC_BUS_ADDR              0x4a338000

#define WRITE_POLL_TIMEOUT              20
#define READ_POLL_TIMEOUT               20

#define STAT_FILE_DATA(file_name)         \
    int                   fd_##file_name; \
    std::ostringstream fname_##file_name;


#define TEST_MALLOC_RES_N_PANISH(ptr)   if(ptr==nullptr) { FPRINTF; exit(-EXIT_FAILURE); }
#define FREE_MEM(ptr)                   if(ptr!=nullptr) { free(ptr); ptr = nullptr; } \
                                        else             { PRINT_ERR("Pointer is NULL.\n"); }

#define MCASP_IN_PCKG_BUF_AMMNT         8

typedef std::queue < std::pair <    void * , size_t > > queue_type;

struct mcasp_data
{
    uint8_t pru_idx;
    uint8_t mcasp_idx;
    uint8_t ts_idx;
    std::mutex * out_mtx_ptr;
    std::mutex *  in_mtx_ptr;
    std::condition_variable *  out_cond_var_ptr;
    std::condition_variable *   in_cond_var_ptr;
    queue_type& out_que;
    queue_type& in_que;
    double& sample_rate;
};

#define DECLARE_MCASP_DATA(PRU_IDX, MCASP_IDX, TS_IDX)       \
std::mutex                   out_mtx_##MCASP_IDX##TS_IDX;    \
std::mutex                    in_mtx_##MCASP_IDX##TS_IDX;    \
std::condition_variable out_cond_var_##MCASP_IDX##TS_IDX;    \
std::condition_variable  in_cond_var_##MCASP_IDX##TS_IDX;    \
queue_type                   out_que_##MCASP_IDX##TS_IDX;    \
queue_type                    in_que_##MCASP_IDX##TS_IDX;    \
double                   sample_rate_##MCASP_IDX##TS_IDX;    \
struct mcasp_data         mcasp_init_##MCASP_IDX##TS_IDX =   \
{                                                            \
     .pru_idx =   PRU_IDX,                                   \
   .mcasp_idx = MCASP_IDX,                                   \
      .ts_idx =    TS_IDX,                                   \
   .out_mtx_ptr      =      &out_mtx_##MCASP_IDX##TS_IDX,    \
    .in_mtx_ptr      =       &in_mtx_##MCASP_IDX##TS_IDX,    \
   .out_cond_var_ptr = &out_cond_var_##MCASP_IDX##TS_IDX,    \
    .in_cond_var_ptr =  &in_cond_var_##MCASP_IDX##TS_IDX,    \
   .out_que          =       out_que_##MCASP_IDX##TS_IDX,    \
    .in_que          =        in_que_##MCASP_IDX##TS_IDX,    \
    .sample_rate     =   sample_rate_##MCASP_IDX##TS_IDX     \
}

#define TO_MCASP_DATA(MCASP_IDX, TS_IDX) mcasp_init_##MCASP_IDX##TS_IDX
extern struct mcasp_data TO_MCASP_DATA(MCASP0, TS0);
extern struct mcasp_data TO_MCASP_DATA(MCASP0, TS1);
extern struct mcasp_data TO_MCASP_DATA(MCASP1, TS0);
extern struct mcasp_data TO_MCASP_DATA(MCASP1, TS1);


class mcasp_ts {
private:
    std::vector < void * >& recv_pckg_bufs_vec_ref;
    struct mcasp_data&      mcasp_data_ref;

    std::ostringstream pru_mcasp_fname;
    int32_t last_send_word = 0;
    /*------------------------------------------------------------------------*/
    bool   stop_write_thread = true;
    std::thread write_thread;
    bool    stop_read_thread = true;
    std::thread  read_thread;
    /*------------------------------------------------------------------------*/

    /* External statistics */
    struct timespec prev_tspec;
    uint32_t        send_pckgs_counter  = 0;
    uint32_t        send_bytes_sum      = 0;
    double          send_stat_period    = 0.0;
    uint32_t        send_bytes_mov_avrg = 0;
    std::queue < std::pair < double , size_t > > send_stat_que;
    double      write_byterate    = 0.0; /* Writing from userland. */
    /*------------------------------------------------------------------------*/

    /* Internal - data from PRU */
    std::ostringstream stat_dir;
    STAT_FILE_DATA(last_tx_msg);
    STAT_FILE_DATA(last_rx_msg);

    std::string last_tx_msg_str;
    std::string last_tx_msg_time_str;
    std::string last_tx_msg_num_str;
    double      last_tx_msg_t_off = 0.0;
    uint32_t    last_tx_msg_num   = 0;
    double      send_byte_rate    = 0.0; /* Internal - data from PRU. */
    /*------------------------------------------------------------------------*/

public:
    int pru_mcasp_fd;

    mcasp_ts ( std::vector < void * >& , struct mcasp_data& );
    ~mcasp_ts( void );

    void start(void);
    void stop (void);

    void    write_sin             (void); /* MCASP test method */

    void    write_mcasp           (void);
    void    write_block_mechanism (void);
    void    write_thread_func     (void);

    void gather_send_stat (size_t);
    void gather_send_stat ();

    void read_thread_func (void);
    /*------------------------------------------------------------------------*/

    void shrink_in_que  (void);
    void shrink_out_que (void);
    /*------------------------------------------------------------------------*/


    /* This members handles necessary for this class facilities.
     * If this buffers would be internal to the class,
     * class destruction would disrupt program.
     * Should be called before constructor. */
    static void   alloc_recv_pckg_buf(std::vector < void * >& );
    static void dealloc_recv_pckg_buf(std::vector < void * >& );
    /*------------------------------------------------------------------------*/
};

#endif
