#ifndef __CHANNEL_H
#define __CHANNEL_H

/*! @file channel.h */

#include "cb.h"

#define CHANNEL_FRAME_SIZE      160
#define FIFO_SIZE               16

struct vocoder;
struct session;
struct s_spi_tdm_device;

struct fifo_stat
{
    unsigned long       write_bytes;
    unsigned long       write_frames;
    unsigned long       write_calls;
    unsigned long       read_bytes;
    unsigned long       read_frames;
    unsigned long       read_calls;
    unsigned long       size;
};

struct channel_stat_param
{
    struct fifo_stat encoder_fifo_stat;
    struct fifo_stat decoder_fifo_stat;
    unsigned long ofw;
    unsigned long ufw;
};

/*! @addtogroup Type_definitions
 * @{ --------------------------------------------------------------------------------------------*/
typedef struct channel
{
    struct session *            link;
    struct s_spi_tdm_device *   spi_tdm;
    struct vocoder *            parent;

    bool        busy;
    u16         tdm;            /*!< timeslot */
    uint        slot;           /*!< channel number */

    /* private */
    long                        encoder_fifo_size;
    long                        encoder_payloadtype;    ///< Look for payload definition in voice-ioctl.h.
    struct s_circular_buffer    encoder_fifo;
    struct fifo_stat            encoder_fifo_stat;

    long                        decoder_fifo_size;
    long                        decoder_upper_watermark;
    long                        decoder_upper_drop_watermark;
    long                        decoder_lower_drop_watermark;
    long                        decoder_lower_watermark;
    long                        decoder_freeze;
    long                        decoder_payloadtype;    ///< Look for payload definition in voice-ioctl.h.
    struct s_circular_buffer    decoder_fifo;
    struct fifo_stat            decoder_fifo_stat;

    char                        marker;

    /*
     *    RTP/SPI    --- write --->  decoder  --- read  --->  TDM
     *    RTP/SPI   <--- read  ---   encoder <--- write ---   TDM
     */

    size_t (*read_enc_frame)    (struct channel *, int * payloadtype, void * buffer, size_t buffer_size);   /*!< read from encoder fifo */
    size_t (*write_enc_frame)   (struct channel *, int   payloadtype, void * buffer, size_t buffer_size);   /*!< write to encoder fifo  */
    size_t (*read_dec_frame)    (struct channel *, int * payloadtype, void * buffer, size_t buffer_size);   /*!< read from decoder fifo */
    size_t (*write_dec_frame)   (struct channel *, int   payloadtype, void * buffer, size_t buffer_size);   /*!< write to decoder fifo  */
    size_t (*decode_frame)      (struct channel *,  void * inbuffer,    size_t inbuffer_size, 
                                                    void * outbuffer,   size_t outbuffer_size); /*!< decode frame */
    size_t (*encode_frame)      (struct channel *,  void * inbuffer,    size_t inbuffer_size, 
                                                    void * outbuffer,   size_t outbuffer_size); /*!< encode frame */

    size_t (*frame_bytes)       (struct channel *);      /*!< count of bytes minimal needed to process */
    size_t (*encoded_bytes)     (struct channel *);      /*!< count of encoded bytes ready to send */
    size_t (*decoded_bytes)     (struct channel *);      /*!< count of decoded bytes ready to send */

    char devname[10];
    
    u32 ofw;
    u32 ufw;
} channel_t;
/*! @} -------------------------------------------------------------------------------------------*/

#define GET_DSP\
    vocoder_t * v = c->parent;\
    dsp_t dsp = v->dsp;


/* Functions prototypes */
char* codec_name(channel_t *);

channel_t * alloc_channel   (unsigned int capabilities);
channel_t * get_channel     (int dev_no, int chn, unsigned int capabilities);
channel_t * get_channel_tdm (int tdm, unsigned int capabilities);

void free_channel       (channel_t *);
void start_channel_tx   (channel_t *);
void stop_channel_tx    (channel_t *);
                                   
void channel_disconnect (channel_t *);
void channel_init       (channel_t *);
void channel_deinit     (channel_t *);

#endif /* __CHANNEL_H */
/*--- End of the file ----------------------------------------------------------------------------*/
