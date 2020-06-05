#ifndef _VMAN2_DSP_H_
#define _VMAN2_DSP_H_

#include <byteswap.h>
#include <linux/ioctl.h>
#include <time.h>
#include "list.h"
#include "hal-ioctl.h"
#include "voice-ioctl.h"
#include "spi-tdm-ioctl.h"

struct dsp_install_info
{
	int 					dsp;
	int 					codec_id;
};

struct dsp_info
{
	struct list_head		list;
	struct param_dsp_owner	info;
};

struct dsp_vad_ts_info
{
	struct list_head		list;
	int						id;
	int						ts;
	int						vad[2];
	int						events_count;
};

struct dsp_vad_info
{
	int						hdsp;
	struct dsproc_event 	event;
	int						id;	
	struct list_head		vad_ts_list;
};

struct s_dsp_channel_info
{
	struct list_head			list;
	int							channel;
	struct timeval				time;
	struct vocoder_stat_param	info;
};

int dsp_list_free(struct list_head	*dsp_list);
int dsp_list_fill(struct list_head	*dsp_list);
int dsp_list_show(FILE * stream, struct list_head	*dsp_list);
int dsp_open(struct dsp_vad_info	*pdvi);
int dsp_close(struct dsp_vad_info	*pdvi);
int dsp_read(struct dsp_vad_info	*pdvi);
int dsp_vad_on(struct dsp_vad_info	*pdvi, int ts);
int dsp_vad_off(struct dsp_vad_info	*pdvi, int ts);
int dsp_vad_show(struct dsp_vad_info	*pdvi, int ts);
int dsp_vad_event(struct dsp_vad_info	*pdvi, struct dsproc_event 	*pevent);

int dsp_install_codec(struct dsp_install_info *info);
int dsp_uninstall_codec(int dsp);
int dsp_channel_info(int channel, struct list_head	*channel_list);

#endif /* _VMAN2_DSP_H_ */

