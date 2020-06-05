#ifndef __USB_GADGET_CLASS__
#define __USB_GADGET_CLASS__

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <wchar.h>
#include <poll.h>
#include <math.h>
#include <time.h>

#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>

#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadgetfs.h>

#include "descriptors.h"
#include "debug.h"
#include "time_manip.hpp"
#ifndef PRINTF
    #define PRINTF(color, text, ...)
    #define PRINT_ERR(text, ...)
    #define FPRINTF
    #define FPRINTF_EXT(text, ...)
#endif
#define SHOW_RECV_STAT              false

#define PAGE_SIZE                   4096
#define USB_PCKG_SIZE_MS            20
#define USB_IN_PCKGS_PER_SECOND     (1000/USB_PCKG_SIZE_MS) /* Used for stat */
#define RD_STAT_MAX_SIZE            10

/* Behavioral */
#define REPACK_TO_4ms_CHUNKS        false
#define USB_IN_PCKG_BUF_AMMNT       16

#define TEST_MALLOC_RES_N_PANISH(ptr)   if(ptr==nullptr) { FPRINTF; exit(-EXIT_FAILURE); }
#define FREE_MEM(ptr)                   if(ptr!=nullptr) { free(ptr); ptr = nullptr; } \
                                        else             { PRINT_ERR("Pointer is NULL.\n"); }

#define MCASP0                      0
#define MCASP1                      1
#define TS0                         0
#define TS1                         1
#define DIR_FROM_USB    0
#define DIR_TO_USB      1

#define MAX_USB_OUT_QUE_SIZE        2

/* Streams name convention */
#define TO_STREAM_IDX(STREAM_DIR,       MCASP_IDX,       TS_IDX) \
                   ( (STREAM_DIR<<2) | (MCASP_IDX<<1) | (TS_IDX << 0) )
#define STREAM_IDX_TO_DIR(STREAM_IDX) (STREAM_IDX>>2)
#define MAX_STREAM_NUM  TO_STREAM_IDX(DIR_TO_USB, MCASP1, TS1)

typedef std::queue < std::pair < void * , size_t > > queue_type;

/*
 Protocol's fields
 #             : 1         | 2           | 3
 -----------------------------------------------------
 Field         : Header    | Data        | CheckSum
 -----------------------------------------------------
 Length, bytes : 16        | 2*msgLength | 2
 -----------------------------------------------------
 Name          : msgHeader | msgData     | msgCheckSum
 */
/*
 Header's fields
 #             : 1               | 2           | 3             | 4             | 5        |
 ------------------------------------------------------------------------------------------
 Field         : Synchronization | Data_length | Packet number | Identificator | Flags    |
 ------------------------------------------------------------------------------------------
 Length, bytes : 4               | 2           | 4             | 2             | 2        |
 ------------------------------------------------------------------------------------------
 Name          : LI_EC__SYNC     | msgLength   | msgSeqNum     | msgID         | msgFlags |
 */
#ifndef LI_GW_HDR_STRUCT_DECLARED
struct LI_GW_header {
    uint32_t msgStart;  /* bytes 0...3 - synchronization constant : 0xE4'E5'AB'2D */
    uint16_t msgLength; /* bytes 4-5   - data length in 16bits words */
    uint32_t msgSeqNum; /* bytes 6..9  - for sequential enumeration of packages curr. type */
    uint16_t msgID;     /* bytes 10-11 - package type/purpose */
    uint16_t msgFlags;  /* bytes 12-13 - service information */
}__attribute__ ((__packed__));
#define LI_GW_HDR_STRUCT_DECLARED
#endif

typedef std::queue < std::pair < double , size_t > > stat_queue_type;
struct recv_stat {
    struct timespec prev_tspec;
    uint32_t        recv_bytes_sum;
    uint32_t        recv_pckgs_counter;
    stat_queue_type recv_stat_que;
    double          recv_stat_period;
    uint32_t        recv_bytes_mov_avrg;
    double          recv_byte_rate;
    double          recv_sample_rate;
};

struct usb_stream
{
    /*!
     * Stream direction   USB->DSP->MCASP == 0
     * Stream direction MCASP->DSP->USB   == 1
     * @{ */
    uint8_t direction : 1;
    /*!@} */
    /*!
     * In current system state it can be 8 streams:
     * 000 - USB->up-sampling->MCASP0 TS0
     * 001 - USB->up-sampling->MCASP0 TS1
     * 010 - USB->up-sampling->MCASP1 TS0
     * " -- "
     * 100 - MCASP0 TS0->down-sampling->USB
     * " -- "
     * 111 - MCASP1 TS1->down-sampling->USB
     * @{ */
    uint8_t number : 4;
    /*!@} */
    uint8_t enable : 1;
    /*! Mutex to protect queue and to synchronize stream handling
     * @{ */
    std::mutex * mtx_ptr;
    /*!@} */
    /*! Signalization between USB and data consumer/producer
     * @{ */
    std::condition_variable * cond_var_ptr;
    /*!@} */
    queue_type&               que;
#if (0)
    double&                   sample_rate; ///<USB !read! sample rate for the correction
#else
    struct recv_stat          r_stat;
#endif
};

/* For USB-> stream data emitted through queue.
 * Mutex and condition variable used for the synchronization.
 * Data rate is the statistics counted on discreet input. */
#define DECLARE_OUT_STREAM(        STREAM_NAME, DIRECTION, MCASP_IDX, TS_IDX)    \
std::mutex                   mtx_##STREAM_NAME##DIRECTION##MCASP_IDX##TS_IDX;    \
std::condition_variable cond_var_##STREAM_NAME##DIRECTION##MCASP_IDX##TS_IDX;    \
queue_type                   que_##STREAM_NAME##DIRECTION##MCASP_IDX##TS_IDX;    \
struct usb_stream         stream_##STREAM_NAME##DIRECTION##MCASP_IDX##TS_IDX =   \
{                                                                                \
    .direction     =                            DIRECTION,                       \
    .number        =                        0b##DIRECTION##MCASP_IDX##TS_IDX,    \
    .enable        =                        0b1,                                 \
    .mtx_ptr       =        &mtx_##STREAM_NAME##DIRECTION##MCASP_IDX##TS_IDX,    \
    .cond_var_ptr  =   &cond_var_##STREAM_NAME##DIRECTION##MCASP_IDX##TS_IDX,    \
    .que           =         que_##STREAM_NAME##DIRECTION##MCASP_IDX##TS_IDX,    \
};

#define DECLARE_IN_STREAM(STREAM_NAME, DIRECTION, MCASP_IDX, TS_IDX,                \
                          in_mtx, in_cond_var, in_que)                              \
struct usb_stream            stream_##STREAM_NAME##DIRECTION##MCASP_IDX##TS_IDX =   \
{                                                                                   \
    .direction        =                        DIRECTION,                           \
    .number           =                        0b##DIRECTION##MCASP_IDX##TS_IDX,    \
    .enable           =                        0b1,                                 \
    .mtx_ptr          = &in_mtx,                                                    \
    .cond_var_ptr     = &in_cond_var,                                               \
    .que              = in_que,                                                     \
};



extern std::vector < struct usb_stream > usb_streams_vector;

class usb_gadget
{
private:
    std::string usb_dev_name           = "/dev/gadget/musb-hdrc";
    std::string usb_to_hst_ep_dev_name = "/dev/gadget/ep1in";
    std::string usb_to_dev_ep_dev_name = "/dev/gadget/ep2out";
    int       ep0_fd = -1;
    int to_hst_ep_fd = -1;
    int to_dev_ep_fd = -1;
    /*------------------------------------------------------------------------*/

    /* Things necessary to create class */
    std::vector < void * >&         recv_pckg_bufs_vec_ref;
    std::vector < struct usb_stream >& usb_streams_vec_ref;
    /*------------------------------------------------------------------------*/

    std::thread   ep0_thread;
    bool     stop_ep0_thread = true;
    std::thread  read_thread;
    bool    stop_read_thread = true;
    std::thread write_thread;
    bool   stop_write_thread = true;
    /*------------------------------------------------------------------------*/

    /* Statistics */
#if (0)
    struct timespec prev_tspec;
    uint32_t        recv_pckgs_counter  = 0;
    uint32_t        recv_bytes_sum      = 0;
    double          recv_stat_period    = 0.0;
    uint32_t        recv_bytes_mov_avrg = 0;
    double          recv_byte_rate      = 0.0; /* Receive data rate in bytes per second. */
    std::queue < std::pair < double , size_t > > recv_stat_que;
#endif

    /* Configuration part */
    int cfg_ep0(void);
    void handle_ep0_thread_func(void);
    void handle_ep0_events  (
                            struct usb_gadgetfs_event * ,
                            int
                            );
    void handle_setup_request(struct usb_ctrlrequest * );
#if (0)
    int handle_ep0(void);
#endif
    /*------------------------------------------------------------------------*/

    /* Read write part */
    int cfg_rd_wr_ep(void);
    void read_thread_func(void);
#if (0)
    void gather_recv_stat(size_t , double& );
#else
    void gather_recv_stat(size_t , struct recv_stat& );
#endif
    void de_LI_GW(  uint8_t * ,
                    size_t& ,
                    uint8_t& ,
                    uint8_t ** );
    uint16_t check_sum_calc( uint8_t * , uint32_t );
    void write_block_mechanism(struct usb_stream& );
    void write_thread_func(void);
    /*------------------------------------------------------------------------*/

    /* Disable copy */
    usb_gadget& operator=(usb_gadget&);

public:
    usb_gadget  (
                std::vector < void * >& ,
                std::vector < struct usb_stream >&
                );
    ~usb_gadget(void);
    /*------------------------------------------------------------------------*/

    /* This members handles necessary for this class facilities.
     * If this buffers would be internal to the class,
     * class destruction would disrupt program.
     * Should be called before constructor. */
    static void   alloc_recv_pckg_buf(std::vector < void * >& );
    static void dealloc_recv_pckg_buf(std::vector < void * >& );
    /*------------------------------------------------------------------------*/
};

#endif /* __USB_GADGET_CLASS__ */
