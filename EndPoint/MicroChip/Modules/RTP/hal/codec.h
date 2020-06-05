#ifndef __CODEC_H
#define __CODEC_H

/*! @file codec.h */

#include "channel.h"
#include "vcodec.h"

struct channel;
struct channel_stat_param;

/*! @addtogroup Type_definitions
 * @{ --------------------------------------------------------------------------------------------*/
typedef struct vocoder {
    dsp_t                   dsp;
    /* private: */
    struct list_head        node;
    struct semaphore        lock;
    struct tasklet_struct   tasklet;
    
    int                     n_chns;
    u8                      id;
    unsigned int            caps;
    
    /// @name DSP related data
    /// @{
    u16     uc_code_ver;
    u16     encoder_fifo_size;
    u16     decoder_fifo_size;
    u16     tdm_offset;         /*!< used by wdt restore function   */
    u16     dtmf_timestamp;     /*!< pointer to dtmf_seq var in DSP */
    /// @}
    
    /// @name mixer
    /// @{
    u16     n_mixers;
    u16     n_inputs;
    u16     gain_base;
    /// @}
    
    /// @name watchdog timers
    /// @{
    struct timer_list wd_timer;
    u16     wd_out_prev;
    u16     wd_count;
    /// @}
    
    /// @name debug info
    /// @{
    uint    total_wd_timeouts;
    uint    total_wd_selftest;
    /// @}
    
    /// @name mixer
    /// @{
    u16    gain[MAX_MIXERS][MAX_MIXER_INPUTS];
    void   (*wd_restore)        (struct vocoder * );                                                                                /*!< restore channel settings after wdt restart */    
    size_t (*read_enc_frame)    (struct channel *, int * payloadtype, void * buffer, size_t buffer_size);                           /*!< read from encoder fifo */
    size_t (*write_enc_frame)   (struct channel *, int   payloadtype, void * buffer, size_t buffer_size);                           /*!< write to encoder fifo  */
    size_t (*read_dec_frame)    (struct channel *, int * payloadtype, void * buffer, size_t buffer_size);                           /*!< read from decoder fifo */
    size_t (*write_dec_frame)   (struct channel *, int   payloadtype, void * buffer, size_t buffer_size);                           /*!< write to decoder fifo  */
    size_t (*frame_bytes)       (channel_t * c);                                                                                    /*!< frame size */
    size_t (*decode_frame)      (struct channel *, void * inbuffer, size_t inbuffer_size, void * outbuffer, size_t outbuffer_size); /*!< decode frame */
    size_t (*encode_frame)      (struct channel *, void * inbuffer, size_t inbuffer_size, void * outbuffer, size_t outbuffer_size); /*!< decode frame */
    
    struct proc_dir_entry *vocoder_proc;
    codec_ops ops;
    /// @}
    
    /// @name channels tail
    /// @{
    struct channel * channels;
    struct channel chan_data[0];
    /// @}
} vocoder_t;
/*! @} -------------------------------------------------------------------------------------------*/

size_t  encoder_fifo_count  (channel_t * c);
size_t  decoder_fifo_count  (channel_t * c);

void g711_table_init(void);
size_t g711_read_enc_frame      (struct channel* c, int * payloadtype, void * buffer, size_t buffer_size);
size_t g711_write_enc_frame     (struct channel* c, int   payloadtype, void * buffer, size_t buffer_size);
size_t g711_read_dec_frame      (struct channel* c, int * payloadtype, void * buffer, size_t buffer_size);
size_t g711_write_dec_frame     (struct channel* c, int   payloadtype, void * buffer, size_t buffer_size);
size_t g711_frame_bytes         (struct channel* c);
size_t g711_decode_frame        (struct channel* c, void * inbuffer, size_t inbuffer_size, void * outbuffer, size_t outbuffer_size);
size_t g711_encode_frame        (struct channel* c, void * inbuffer, size_t inbuffer_size, void * outbuffer, size_t outbuffer_size);

size_t lin16_read_enc_frame     (struct channel* c, int * payloadtype, void * buffer, size_t buffer_size);
size_t lin16_write_enc_frame    (struct channel* c, int   payloadtype, void * buffer, size_t buffer_size);
size_t lin16_read_dec_frame     (struct channel* c, int * payloadtype, void * buffer, size_t buffer_size);
size_t lin16_write_dec_frame    (struct channel* c, int   payloadtype, void * buffer, size_t buffer_size);
size_t lin16_frame_bytes        (struct channel* c);
size_t lin16_decode_frame       (struct channel* c, void * inbuffer, size_t inbuffer_size, void * outbuffer, size_t outbuffer_size);
size_t lin16_encode_frame       (struct channel* c, void * inbuffer, size_t inbuffer_size, void * outbuffer, size_t outbuffer_size);

size_t dummy_read_enc_frame     (struct channel* c, int * payloadtype, void * buffer, size_t buffer_size);
size_t dummy_write_enc_frame    (struct channel* c, int   payloadtype, void * buffer, size_t buffer_size);
size_t dummy_read_dec_frame     (struct channel* c, int * payloadtype, void * buffer, size_t buffer_size);
size_t dummy_write_dec_frame    (struct channel* c, int   payloadtype, void * buffer, size_t buffer_size);
size_t dummy_frame_bytes        (struct channel* c);
size_t dummy_decode_frame       (struct channel* c, void * inbuffer, size_t inbuffer_size, void * outbuffer, size_t outbuffer_size);
size_t dummy_encode_frame       (struct channel* c, void * inbuffer, size_t inbuffer_size, void * outbuffer, size_t outbuffer_size);

int     jb_monitor  (channel_t *);
int     jb_reset    (channel_t *);

/*! @addtogroup Exported Exported to kernel functions
 * @{ --------------------------------------------------------------------------------------------*/
int         vocoder_remove              (vocoder_t * v);
int         vocoder_install             (int dspno, int id);
int         vocoder_uninstall           (int dspno);
int         vocoders_init               (void * context);
void        vocoders_deinit             (void * context);
vocoder_t * vocoder_find                (int dspno);
int         vocoder_get_num_channels    (int id);
int         vocoder_alloc_channel       (int id);
int         vocoder_stat_channel        (int channel, struct channel_stat_param * info);

char *          codec_name_from_id  (unsigned short codec_id );
unsigned int    caps_from_pt        (unsigned short rx_pt, unsigned short tx_pt);
/*! @} -------------------------------------------------------------------------------------------*/

/*! @name Initialization facilities
 * @{ */
int init_mixers(void);
void cleanup_mixers(void);
void mixer_restore(vocoder_t *);

int spi_tdm_init(void *context);
void spi_tdm_deinit(void *context);
/*! @} -------------------------------------------------------------------------------------------*/

#endif
/*--- End of the file ----------------------------------------------------------------------------*/
