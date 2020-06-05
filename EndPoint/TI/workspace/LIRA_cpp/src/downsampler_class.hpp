#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <math.h>

#include <thread>
#include <cmath>
#include <queue>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ctime>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>


#include "soxr-lsr.h"
#include "soxr.h"
#include "g711.h"

/*----------------------------------------------------------------------------*/
#include "debug.h"
#ifndef PRINTF
    #define PRINTF(color, text, ...)
    #define PRINT_ERR(text, ...)
    #define FPRINTF
    #define FPRINTF_EXT(text, ...)
    #define PRINT_COND(condition, color, text, ... )
#endif
#define SHOW_TO_USB_QUE_SHRINK          false

#define PAGE_SIZE                       4096

#define MCASP_IN_BUF_SIZE_4ms_WORDS     64 /* 16000Hz 16(32) bit   4ms == 4x16 = 64w */
#define MCASP_IN_BUF_SIZE_4ms_BYTES     ( MCASP_IN_BUF_SIZE_4ms_WORDS * sizeof (int32_t) )

#define USB_OUT_BUF_SIZE_4ms_WORDS      32 /* alaw 8000Hz 8 bit 4ms == 4*8 = 32w */
#define USB_OUT_BUF_SIZE_4ms_BYTES      ( USB_OUT_BUF_SIZE_4ms_WORDS * sizeof (uint8_t) )

#define USB_OUT_BUF_SIZE_20ms_BYTES     ( USB_OUT_BUF_SIZE_4ms_BYTES * 5 )

#define TEST_MALLOC_RES_N_PANISH(ptr)   if(ptr==nullptr) { FPRINTF; exit(-EXIT_FAILURE); }
#define FREE_MEM(ptr)                   if(ptr!=nullptr) { free(ptr); ptr = nullptr; } \
                                        else             { PRINT_ERR("Pointer is NULL.\n"); }

#define AUDIO_DATA_BUF_AMMNT            16
#define MAX_TO_USB_QUEUE_SIZE           2

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

#define LI_EC_HDR_SIZE                              sizeof(struct LI_EC_header)
#define LI_EC_CRC_SIZE                              (sizeof(uint16_t))

struct downsampler_data
{
    std::vector < void * >&  out_pckg_bufs; /* Buffers for output LI_GW packages */
    queue_type&  in_que; /* MCASP - I2S 16kHz 16(32)bit */
    queue_type& out_que; /* USB - alaw 8kHz 8bit */
    /* Synchronization mechanisms */
    std::condition_variable *  in_cond_var_ptr; /* From MCASP */
    std::condition_variable * out_cond_var_ptr; /* To USB */
    std::mutex *  in_mtx_ptr; /* From MCASP */
    std::mutex * out_mtx_ptr; /* To USB */
    /* For LI_GW */
    uint8_t mcasp_idx;
    uint8_t ts_idx;
    uint8_t dev_num; /* From switch on the PCB */
    /* For soxr */
    double  input_rate;
    double output_rate;
};

class downsampler
{
private:
    /* Things necessary to create class */
    downsampler_data& ddata_ref;
    /*------------------------------------------------------------------------*/
    std::thread      aproc_thread;
    bool        stop_aproc_thread = true;
    /*------------------------------------------------------------------------*/
    soxr_t            soxr;
    int16_t *  in_buf_soxr_connector;
    int16_t * out_buf_soxr_connector;

public:
    downsampler ( downsampler_data& ddata_ref );
    ~downsampler();
    static void   alloc_out_pckg_buf(std::vector < void * >& );
    static void dealloc_out_pckg_buf(std::vector < void * >& );

    void start (void);
    void stop  (void);
    /*------------------------------------------------------------------------*/
    static uint16_t calc_and_settle_CRC( uint8_t * , size_t  );
    void LI_GW_pckgr( uint8_t * , size_t );
    uint8_t * FTDI_packager(uint8_t * pckg_buf, size_t& pckg_len_ref);
    /*------------------------------------------------------------------------*/
    soxr_error_t prepare_soxr(void);
    void block_mechanism(void);
    void downsampler_thread_func(void);
    soxr_error_t soxr_wrapper (int32_t * in_buf_ptr, int16_t * out_buf_ptr);

};
