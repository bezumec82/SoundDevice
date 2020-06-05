/*
 * $Id: voice-ioctl.h,v 1.1 2006/08/31 15:11:16 root Exp $
 */


#ifndef __VOICE_IOCTL_H
#define __VOICE_IOCTL_H

#define IN
#define OUT

#define VOCODER_BASE	'V'
#define CODEC_BASE		'C'
#define TONEC_BASE		'T'
#define RTP_BASE		'R'
#define RTP_EXT_BASE	'E'
#define DEBUG_BASE		'D'
#define MIX_BASE		'M'
#define PLAYREC_BASE	'P'

/* *************************************************************** */
/* vocoder-related structures and ioctls */
#define VOICE_DEVNAME "voice"

#define VOCODER_INSTALL			_IOW(VOCODER_BASE, 0, struct vocoder_install_param)
#define VOCODER_UNINSTALL		_IOW(VOCODER_BASE, 1, unsigned int)
#define VOCODER_NUM_CHANNELS	_IOR(VOCODER_BASE, 2, unsigned int)
#define VOCODER_ALLOC_CHANNEL	_IOR(VOCODER_BASE, 3, unsigned int)
#define VOCODER_STAT_CHANNEL	_IOWR(VOCODER_BASE, 16, struct vocoder_stat_param)
/* find appropriate channel from CODEC_ID */

#define VOCODER_DRV_VERSION		_IOR(VOCODER_BASE, 10, struct drv_version)


#define	TONEC_CED		0x11
#define	TONEC_CNG		0x12

struct vocoder_install_param
{
	IN unsigned int dsp;
	IN unsigned int codec_id;
};

struct drv_version
{
	OUT unsigned board_major;
	OUT unsigned board_minor;

	OUT unsigned drv_major;
	OUT unsigned drv_minor;
	OUT unsigned drv_build;
};

struct channel_fifo_stat
{
	unsigned long		write_bytes;
	unsigned long		write_frames;
	unsigned long		write_calls;
	unsigned long		read_bytes;
	unsigned long		read_frames;
	unsigned long		read_calls;
	unsigned long		size;
};

struct vocoder_stat_param
{
	IN  unsigned int channel;
	OUT struct channel_fifo_stat encoder_fifo_stat;
	OUT struct channel_fifo_stat decoder_fifo_stat;
	OUT unsigned long ofw;
	OUT unsigned long ufw;
};

/* ************************************************************ */
/*						R T P									*/
/* ************************************************************ */

/* rtp-related structures and ioctls */
#define RTP_DEVNAME	"rtp"


#define SUBBUFFER_COUNT			2
/* 10 ms frame for G.711	*/
#define G711_DSP_SUBFRAME_SIZE		40
#define G711_PCM_SUBFRAME_SIZE		40
#define G711_DSP_FRAME_SIZE			(SUBBUFFER_COUNT*G711_DSP_SUBFRAME_SIZE)
#define G711_PCM_FRAME_SIZE			(SUBBUFFER_COUNT*G711_PCM_SUBFRAME_SIZE)
/* 10 ms frame for LIN16	*/
#define LIN16_DSP_SUBFRAME_SIZE		80
#define LIN16_PCM_SUBFRAME_SIZE 	80
#define LIN16_DSP_FRAME_SIZE		(SUBBUFFER_COUNT*LIN16_DSP_SUBFRAME_SIZE)
#define LIN16_PCM_FRAME_SIZE	 	(SUBBUFFER_COUNT*LIN16_PCM_SUBFRAME_SIZE)


/* payload types */
#define PAYLOAD_TYPE_G711U		0
#define PAYLOAD_TYPE_G723		4
#define PAYLOAD_TYPE_G711A		8
#define PAYLOAD_TYPE_G729		18
/* obsolete values, don't use it!	*/
#define PAYLOAD_TYPE_G711A_MCC	(PAYLOAD_TYPE_G711A | 0x80)
#define PAYLOAD_TYPE_G711U_MCC	(PAYLOAD_TYPE_G711U | 0x80)
#define PAYLOAD_TYPE_LIN16		0x7F

struct rtp_connection_params
{
	IN unsigned long send_addr;			/* network byte order, i.e. MSB first		*/
	IN unsigned short send_port;        /* host byte order, i.e. LSB first			*/
	IN unsigned long recv_addr;			/* network byte order, i.e. MSB first		*/
	IN unsigned short recv_port;		/* host byte order, i.e. LSB first			*/
	IN unsigned short rx_pt;			/* payload type for receive from network	*/
	IN unsigned short tx_pt;			/* payload type for transmit to network		*/
	IN unsigned short packet_factor;	/* for all codec types one is 10ms, except
										   G.723.1, where is 30ms. If zero - driver
										   set the default value					*/
	IN unsigned char vad;				/* VAD/CNG enable for transmit side			*/
	IN unsigned char rate;				/* 5.3 or 6.3 kbps rate for G.723.1 codec	*/
	IN unsigned int ssrc;				/* RTP session source ID, if 0 then
										   generated randomly						*/
	IN unsigned int jb_initial;			/* RX buffer capacity if initial phase		*/
	IN unsigned int jb_max;				/* RX buffer drop level border				*/
};

struct rtp_channel_info
{
	int tdm;
	int dev_no;
	int chn;
};

struct rtp_filter_params
{
	IN unsigned long recv_addr;			/* remote host IP address , if 0 then not inuse */
	IN unsigned long ssrc;				/* SSRC RTP session, if 0 then not inuse */
};

struct rtp_stat_info
{
	unsigned long		ptx;
	unsigned long		prx;
	unsigned long		tx_dropped;
	unsigned long		rx_dropped;
	unsigned long 		tx_last;	/* ms */
	unsigned long 		rx_last;	/* ms */
};

#define RTP_BEGIN_SESSION			_IOWR(RTP_BASE, 0, struct rtp_connection_params)
/* Open socket with specified params and prepare RTP threads */

#define RTP_STOP_SESSION			_IO(RTP_BASE, 1)
/* Release socket and free all channels */

#define RTP_ADD_CHANNEL				_IOWR(RTP_BASE, 2, struct rtp_channel_info)
/*	Add channel to session (bundle in future)
	allocate an appropriate codec. If you wont add specified channel,
	fill rtp_channel_info struct with follow rules:
   	First driver check 'tdm' for positive value, else it checked pair 'dev_no':'chn'.
   	If 'chn' is negative then driver will use 'dev_no' as preffered device for allocate.
   	If 'chn' is positive then driver called method GET(device, channel) in forced mode.
   	After allocate, driver complete fill struct.
   	the 'struct rtp_channel_info' may be extended in future.
 */

#define RTP_REMOVE_CHANNEL			_IO(RTP_BASE, 3)
/* Remove specified channel from session */

#define RTP_START_TX				_IO(RTP_BASE, 4)
/* Start transmit session thread */

#define RTP_STOP_TX					_IO(RTP_BASE, 5)
/* Stop transmit session thread */

#define RTP_SET_FILTER				_IOWR(RTP_BASE, 6, struct rtp_filter_params)
/* Set rtp filter */

#define RTP_RESET_FILTER			_IO(RTP_BASE, 7)
/* Reset rtp filter */

#define RTP_STAT_INFO				_IOWR(RTP_BASE, 8, struct rtp_stat_info)
/* Get RTP statistic */

/* ************************************************************ */
/*						R T P    E X T E N S I O N				*/
/* ************************************************************ */

/* rtp-related structures and ioctls */
#define RTP_EXT_DEVNAME	"rtp_ext"

struct rtp_ext_connection_params
{
	IN unsigned short	etype;			/* extension type */
	IN int				fd_rtp;			/* file descriptor of RTP session */
};

struct rtp_ext_source_addr
{
	OUT unsigned long	ssrc;			/* RTP SSRC from incoming RTP session */
	OUT unsigned long	src_addr;		/* remote host ip from incoming RTP session */
	OUT unsigned short	src_port;		/* remote host port from incoming RTP session */
	OUT unsigned short	etype;			/* extension type */
};

#define RTP_EXT_ATTACH_SESSION			_IOWR(RTP_EXT_BASE, 0, struct rtp_ext_connection_params)
#define RTP_EXT_DETACH_SESSION			_IO(RTP_EXT_BASE, 1)
#define RTP_EXT_READ_SOURCE_ADDR		_IOWR(RTP_EXT_BASE, 2, struct rtp_ext_source_addr)

/* ****************************************************************	*/
/* 		mixer-related structures and ioctls							*/
/* ****************************************************************	*/

#define MIXER_DEVNAME	"mixer"
#define MIX_TX_MODE		_IOW(MIX_BASE, 0, struct mix_tx_mode)
#define MIX_RX_CLEAR	_IOW(MIX_BASE, 2, struct mix_slot)
#define MIX_RX_ADD		_IOW(MIX_BASE, 3, struct mix_slot_pair)
#define MIX_RX_REMOVE	_IOW(MIX_BASE, 4, struct mix_slot_pair)
#define MIX_QUERY		_IOWR(MIX_BASE, 5, struct mix_query)
#define MIX_TDM			_IOW(MIX_BASE, 6, struct mix_slot)
#define MIX_ENUM_DEVS	_IOW(MIX_BASE, 7, struct mix_enum)

enum slot_type
{
	slot_tdm    = 0,
	slot_packet = 1
};

struct mix_slot
{
	int dev_no;
	enum slot_type type;
	int number;
};

struct  mix_slot_pair
{
	struct mix_slot mixer;	// mixer (summing device, from 0 to MAX_MIXERS-1)
	struct mix_slot input;	// entry point to correspondent slot (from 0 to MAX_INPUTS-1)
	int gain;				// input gain (0-32767)
};

struct mix_query
{
	IN int dev_no;
	IN int mixer;
	IN int entry;
	OUT int type;
	OUT int addr;
	OUT int port;
	OUT int gain;
};

struct mix_enum
{
	OUT int number;
	OUT int tdm_channels;
	OUT int packet_channels;
	OUT int channels_per_conf;
};

/* for information only */

#define CODEC_ID_G711		0x0001
#define CODEC_ID_G723		0x0002
#define CODEC_ID_G729		0x0004
#define CODEC_ID_G711_MCC	0x0008
#define CODEC_ID_MODEM		0x0010
#define CODEC_ID_MIXER		0x0020
#define CODEC_ID_CMIXER		0x0040
#define CODEC_ID_LIN16		0x0080


#endif


