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
#define SHOW_SINC_TABLE                 false
#define SHOW_CORRECTION                 false
#define SHOW_CORRECTION_SEVERE          false
/*----------------------------------------------------------------------------*/

#define USB_IN_BUF_SIZE_4ms_WORDS          32 /* alaw 8000Hz 8 bit   4ms == 32w */
#define USB_IN_BUF_SIZE_4ms_BYTES          (uint32_t)( USB_IN_BUF_SIZE_4ms_WORDS * sizeof (uint8_t) )
/* For correction tables */
#define CORRECTION_PER_PCKG                 1 /* Should be aliquot to pckg. size. */
#define USB_IN_BUF_SIZE_4ms_WORDS_INDCD     (USB_IN_BUF_SIZE_4ms_WORDS + CORRECTION_PER_PCKG)
#define USB_IN_BUF_SIZE_4ms_WORDS_REDCD     (USB_IN_BUF_SIZE_4ms_WORDS - CORRECTION_PER_PCKG)

#define MCASP_OUT_BUF_SIZE_4ms_WORDS        64 /* 16000Hz 32 bit 4ms == 64w */
#define MCASP_OUT_BUF_SIZE_4ms_BYTES        (uint32_t)( MCASP_OUT_BUF_SIZE_4ms_WORDS * sizeof (int32_t) )

#define RANGE                               256
#define IN_SIGNAL_MIN                       -128

#define PCKGS_PER_SEC                   (1000/4) /* One pckg. 4ms length. */

#define AUDIO_DATA_BUF_AMMNT            16
#define PAGE_SIZE                       4096

#define CORRECTION_THREASHOLD           (CORRECTION_PER_PCKG * PCKGS_PER_SEC)
#define NORM_OUT_QUEUE_SIZE             5 /* One 20ms pckg it is a 5 pckgs by 4ms */
#define MAX_OUT_QUEUE_SIZE              (NORM_OUT_QUEUE_SIZE + 1) /* Take accumulator buffer into account */
#define MIN_OUT_QUEUE_SIZE              5

/* Behavioral */
#define SINC_PROCESSING                 true
#define SAMPLERATE_CORRECTION           true
#define TABLE_POW                       15

#define TEST_MALLOC_RES_N_PANISH(ptr)   if(ptr==nullptr) { FPRINTF; exit(-EXIT_FAILURE); }
#define FREE_MEM(ptr)                   if(ptr!=nullptr) { free(ptr); ptr = nullptr; } \
                                        else             { PRINT_ERR("Pointer is NULL.\n"); }
/*----------------------------------------------------------------------------*/

typedef std::queue < std::pair < void * , size_t > > queue_type;

class upsampler
{
private:
    /* Things necessary to create class */
    std::vector < void * >&  out_pckg_bufs;
    queue_type&              in_que;  /* alaw 8000Hz input */
    queue_type&              out_que; /* i2s 16000Hz output */
    /* Byterate correction facilities */
    double&                  in_smpl_rate;
    double&                  out_smpl_rate;
    /* Synchronization mechanism */
    std::condition_variable *  in_cond_var_ptr = nullptr;
    std::condition_variable * out_cond_var_ptr = nullptr;
    std::mutex *  in_mtx_ptr = nullptr;
    std::mutex * out_mtx_ptr = nullptr;
    /*------------------------------------------------------------------------*/
    std::thread      aproc_thread;
    bool        stop_aproc_thread = true;
    /*------------------------------------------------------------------------*/
    soxr_t            soxr;
    int16_t *  in_buf_soxr_connector;
    int16_t * out_buf_soxr_connector;
    /*------------------------------------------------------------------------*/
    double *** sinc_tbl_256x33x32 = nullptr;
    double *** sinc_tbl_256x31x32 = nullptr;

    int32_t **  sinc_tbl_33x32 = nullptr;
    int32_t **  sinc_tbl_31x32 = nullptr;
    /*------------------------------------------------------------------------*/
public:
    upsampler   (
                std::vector < void * >& ,
                queue_type& ,
                queue_type& ,
                double& ,
                double&
                );
    ~upsampler();
    static void   alloc_out_pckg_buf(std::vector < void * >& );
    static void dealloc_out_pckg_buf(std::vector < void * >& );

    void start (void);
    void stop  (void);
    void allocate_sinc_tbl_256x33x32(void);
    void allocate_sinc_tbl_256x31x32(void);

    void deallocate_sinc_tbl_256x33x32(void);
    void deallocate_sinc_tbl_256x31x32(void);

    void allocate_sinc_tbl_33x32(void);
    void allocate_sinc_tbl_31x32(void);

    void deallocate_sinc_tbl_33x32(void);
    void deallocate_sinc_tbl_31x32(void);
    /*------------------------------------------------------------------------*/

    void set_in_sync(std::condition_variable * in_cond_var_ptr,
                     std::mutex *              in_mtx_ptr)
    {
        this->in_cond_var_ptr = in_cond_var_ptr;
        this->in_mtx_ptr = in_mtx_ptr;
    }
    void set_out_sync(std::condition_variable * out_cond_var_ptr,
                      std::mutex *              out_mtx_ptr)
    {
        this->out_cond_var_ptr = out_cond_var_ptr;
        this->out_mtx_ptr = out_mtx_ptr;
    }
    /*------------------------------------------------------------------------*/
    soxr_error_t prepare_soxr(void);
    /*------------------------------------------------------------------------*/
    void block_mechanism(void);
    void upsampler_thread_func(void);
    void resample_8x8000_to_16x16000    (
                                        int8_t *  in_buf_ptr,
                                        int32_t * out_buf_ptr,
                                        size_t    in_buf_size,
                                        size_t    out_buf_size
                                        );
    void data2sinc_rebuilder(
                int8_t * in_buf,  size_t in_buf_size,
                int8_t * out_buf, size_t out_buf_size
                );
    void data2sinc_rebuilder(
                int16_t * in_buf,  size_t in_buf_size,
                int16_t * out_buf, size_t out_buf_size
                );
    soxr_error_t soxr_wrapper (int8_t *  in_buf_ptr, int32_t * out_buf_ptr);
    soxr_error_t soxr_wrapper (int16_t * in_buf_ptr, int32_t * out_buf_ptr);
};
