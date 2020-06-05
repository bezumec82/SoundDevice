#ifndef _VCODEC_H
#define _VCODEC_H


#define MODE_LMS            0x0001
#define MODE_VAD            0x0010
#define MODE_HIGH_RATE      0x0020
#define MODE_ENCODER_G711   0x0100
#define MODE_ENCODER_G711A  MODE_ENCODER_G711
#define MODE_ENCODER_G711U  (MODE_ENCODER_G711 | 0x0200)
#define MODE_DECODER_G711   0x0400
#define MODE_DECODER_G711A  MODE_DECODER_G711
#define MODE_DECODER_G711U  (MODE_DECODER_G711 | 0x0800)

#define CAP_G711            0x0001
#define CAP_G723            0x0002
#define CAP_G729            0x0004
#define CAP_G711U           0x0008
#define CAP_MIXER           0x0800
#define CAP_MCC             0x1000
#define CAP_LIN16           0x2000

#define CAP_ALL_CODECS (CAP_G711 | CAP_G723 | CAP_G729)

#define CODEC_ID_G711       0x0001
#define CODEC_ID_G723       0x0002
#define CODEC_ID_G729       0x0004
#define CODEC_ID_G711_MCC   0x0008
#define CODEC_ID_MODEM      0x0010
#define CODEC_ID_MIXER      0x0020
#define CODEC_ID_CMIXER     0x0040
#define CODEC_ID_LIN16      0x0080

#define VOC_STATUS_OK       0
#define VOC_STATUS_DISABLED 1
#define MAX_MIXER_INPUTS    16
#define MAX_MIXERS          32

#define VCODEC_PROGRAM_VERSION  0x0001
#define VCODEC_NUM_OF_CHANNELS  8
#define VCODEC_CAPABILITIES         (CAP_G711 | CAP_G711U | CAP_LIN16 | CAP_MIXER)
#define VCODEC_G711_CAPABILITIES    (CAP_G711 | CAP_G711U)
#define VCODEC_LIN16_CAPABILITIES   (CAP_LIN16)
    
struct vcodec_info
{
    unsigned short  Capabilities;
    unsigned short  ProgramVersion;
    unsigned short  NumberOfChannels;
    unsigned short  decoder_fifo_size;
    unsigned short  encoder_fifo_size;
};

int     read_channels_count     (dsp_t dsp);
void    read_exchange_block     (struct vocoder* v);
void    read_channel_data       (struct channel*);
void    virtual_codec_install   (dsp_t dsp);
void    virtual_codec_uninstall (dsp_t dsp);

#endif
