#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>

#include "dsp.h"
#include "file.h"

#if 1
#define DSP_DEBUG(args...) fprintf(stderr, args)
#else
#define DSP_DEBUG(args...)
#endif

/* -------------------------------------------------------------------------- */
long _timevaldiff(struct timeval *starttime, struct timeval *finishtime)
{
  long msec;
  msec=(finishtime->tv_sec - starttime->tv_sec)*1000;
  msec+=(finishtime->tv_usec-starttime->tv_usec)/1000;
  return msec;
}
/* -------------------------------------------------------------------------- */
int dsp_list_free(struct list_head	*dsp_list)
{
	struct dsp_info	*pdi;
	struct list_head *pos,*tmp;
	if (dsp_list)
	{
		list_for_each_safe(pos, tmp, dsp_list)
		{
			pdi = list_entry(pos, struct dsp_info, list);
			list_del(pos);
			free(pdi);
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int dsp_list_fill(struct list_head	*dsp_list)
{
	struct param_dsp_owner owner;
	struct dsp_info* temp;
	int i, n_dsp, dsp_list_length = 0;
	int res;
	int hal_ctl;

	if (dsp_list)
	{
		dsp_list_free(dsp_list);
		
		hal_ctl = open_hal();
		if (hal_ctl < 0)
		{
			fprintf(stderr, "error: open hal '%s(%d)'", strerror(errno), errno);
			fflush(stderr);
		}
		else
		{
			res = do_ioctl(hal_ctl, HAL_DSP_GET_MAX, &n_dsp, "hal_dsp_get_max");

			for (i = 0; i < n_dsp; i++)
			{
				owner.number = i;
				if (ioctl(hal_ctl, HAL_DSP_OWNER, &owner) >= 0)
				{
					temp = malloc( sizeof(struct dsp_info) );
					if( temp )
					{
						temp->info = owner;
						list_add(&(temp->list), dsp_list);
					}
					else
					{
						printf( "can't create dsp_info instance!\n" );					
					}
				}
			}
			
			close(hal_ctl);
		}
	}
	
	return dsp_list_length;
}
/* -------------------------------------------------------------------------- */
int dsp_list_show(FILE * stream, struct list_head	*dsp_list)
{
	struct list_head *pos, *tmp;
	struct dsp_info* pdi;
	struct param_dsp_owner* pd;
	if (stream)
	{
		fprintf(stream, "DSP map:\n");
		if (dsp_list)
		{
			list_for_each_safe(pos, tmp, dsp_list)
			{
				pdi = list_entry(pos, struct dsp_info, list);
				
				pd = &pdi->info;
				if( pd->owner_name[0]=='\0' )
					fprintf(stream, "\tdsp%02d: %s\tbus=%d\n", pd->number, "-----", pd->bus );
				else
					fprintf(stream, "\tdsp%02d: %s\tbus=%d\tts=[%d:%d]\n", pd->number, pd->owner_name, pd->bus, pd->ts_base, pd->ts_base + pd->ts_count - 1);
				
			}
		}
		fflush(stream);
	}
	
	return 0;
}
/* -------------------------------------------------------------------------- */
int dsp_open(struct dsp_vad_info	*pdvi)
{
	if (pdvi)
	{
		pdvi->hdsp = open_dsp();
		if (pdvi->hdsp < 0)
		{
			fprintf(stderr, "error: open dsp: '%s'(%d)\n", strerror(errno), errno);
			fflush(stderr);
		}
		pdvi->id = 1;
		INIT_LIST_HEAD(&pdvi->vad_ts_list);
	}
	return pdvi->hdsp;
}
/* -------------------------------------------------------------------------- */
int dsp_close(struct dsp_vad_info	*pdvi)
{
	if (pdvi)
	{
		dsp_vad_off(pdvi, -1);
		close(pdvi->hdsp);
		pdvi->hdsp = -1;		
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int dsp_read(struct dsp_vad_info	*pdvi)
{
	int nread = 0;
	if (pdvi)
	{
		nread = read(pdvi->hdsp, &pdvi->event, sizeof(pdvi->event));
		if (nread < 0)
		{
			fprintf(stderr, "error: open dsp: '%s'(%d)\n", strerror(errno), errno);
			fflush(stderr);
		}
		else
		{
			if (nread > 0)
			{
				dsp_vad_event(pdvi, &pdvi->event);
				
				DSP_DEBUG("read dsp event\n");
				DSP_DEBUG("ts=%d\n", pdvi->event.ts);
				DSP_DEBUG("id=%d\n", pdvi->event.id);
				DSP_DEBUG("type=%d\n", pdvi->event.dsproc_type);
				if (pdvi->event.dsproc_type == DSPROC_TYPE_VAD)
				{
					DSP_DEBUG("vad.mask=0x%02X\n", pdvi->event.payload.vad.dir_mask);
					DSP_DEBUG("vad.rx=%d\n", pdvi->event.payload.vad.value[VAD_DIR_RX]);
					DSP_DEBUG("vad.tx=%d\n", pdvi->event.payload.vad.value[VAD_DIR_TX]);
				}
				fflush(stdout);
			}
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int dsp_vad_on(struct dsp_vad_info	*pdvi, int ts)
{
	struct dsp_vad_ts_info	*pvti;
	struct dsproc_enable	de;
	int res;
	
	if (pdvi)
	{
		de.ts = ts;
		de.id = pdvi->id++;
		de.dsproc_type_mask = VAD_DIR_RX_MASK | VAD_DIR_TX_MASK;
		res = do_ioctl(pdvi->hdsp, DSPROC_ENABLE, &de, "dsp_vad_enable");
		
		pvti = malloc(sizeof(struct dsp_vad_ts_info));
		pvti->ts = de.ts;
		pvti->id = de.id;
		pvti->vad[0] = -1;
		pvti->vad[1] = -1;
		pvti->events_count = 0;
		
		list_add(&(pvti->list), &(pdvi->vad_ts_list));
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int dsp_vad_off(struct dsp_vad_info	*pdvi, int ts)
{
	struct list_head *pos,*tmp;
	struct dsp_vad_ts_info	*pvti;
	struct dsproc_enable	de;
	int res;
	
	if (pdvi)
	{
		list_for_each_safe(pos, tmp, &(pdvi->vad_ts_list))
		{
			pvti= list_entry(pos, struct dsp_vad_ts_info, list);
			if ((pvti->ts == ts) || (ts <= 0))
			{
				fprintf(stdout, "Disable DSP VAD on timeslot=%d\n", pvti->ts);
				de.ts = pvti->ts;
				de.id = pdvi->id;
				de.dsproc_type_mask = VAD_DIR_RX_MASK | VAD_DIR_TX_MASK;
				res = do_ioctl(pdvi->hdsp, DSPROC_DISABLE, &de, "dsp_vad_enable");
				
				list_del(pos);
				free(pvti);
			}
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int dsp_vad_show(struct dsp_vad_info	*pdvi, int ts)
{
	struct list_head *pos,*tmp;
	struct dsp_vad_ts_info	*pvti;
	
	if (pdvi)
	{
		fprintf(stdout, "DSP VADs:\n");
		list_for_each_safe(pos, tmp, &(pdvi->vad_ts_list))
		{
			pvti= list_entry(pos, struct dsp_vad_ts_info, list);
			if ((pvti->ts == ts) || (ts <= 0))
			{			
				fprintf(stdout, "\n");
				fprintf(stdout, "              ts:%d\n", pvti->ts);
				fprintf(stdout, "              id:%d\n", pvti->id);
				fprintf(stdout, "          vad.rx:%d\n", pvti->vad[VAD_DIR_RX]);
				fprintf(stdout, "          vad.tx:%d\n", pvti->vad[VAD_DIR_TX]);
				fprintf(stdout, "    events count:%d\n", pvti->events_count);
			}
		}
		fflush(stdout);
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int dsp_vad_event(struct dsp_vad_info	*pdvi, struct dsproc_event 	*pevent)
{
	struct list_head *pos,*tmp;
	struct dsp_vad_ts_info	*pvti;
	
	if (pdvi && pevent)
	{
		list_for_each_safe(pos, tmp, &(pdvi->vad_ts_list))
		{
			pvti= list_entry(pos, struct dsp_vad_ts_info, list);
			if ((pvti->ts == pevent->ts) && (pvti->id == pevent->id))
			{
				if (pdvi->event.dsproc_type == DSPROC_TYPE_VAD)
				{
					pvti->events_count++;
					if (pdvi->event.payload.vad.dir_mask & VAD_DIR_RX_MASK)
						pvti->vad[VAD_DIR_RX] = pevent->payload.vad.value[VAD_DIR_RX];
					
					if (pdvi->event.payload.vad.dir_mask & VAD_DIR_TX_MASK)
						pvti->vad[VAD_DIR_TX] = pevent->payload.vad.value[VAD_DIR_TX];
				}
			}
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int dsp_install_codec(struct dsp_install_info *info)
{
	struct vocoder_install_param ios;
	int voice_ctl;
	int res=-1;
	voice_ctl = open_ctl();
	if (voice_ctl < 0)
	{
		fprintf(stderr, "error: open voice_ctl '%s(%d)'", strerror(errno), errno);
		fflush(stderr);
	}
	else
	{
		ios.dsp			= info->dsp;
		ios.codec_id	= info->codec_id;
		res = do_ioctl(voice_ctl, VOCODER_INSTALL, &ios, "install vocoder");
		close(voice_ctl);
	}
	return res;
}
/* -------------------------------------------------------------------------- */
int dsp_uninstall_codec(int dsp)
{
	int voice_ctl;
	int res=-1;
	voice_ctl = open_ctl();
	if (voice_ctl < 0)
	{
		fprintf(stderr, "error: open voice_ctl '%s(%d)'", strerror(errno), errno);
		fflush(stderr);
	}
	else
	{
		res = do_ioctl(voice_ctl, VOCODER_UNINSTALL, &dsp, "uninstall vocoder");
		close(voice_ctl);
	}
	return res;
}
/* -------------------------------------------------------------------------- */
int dsp_channel_info(int channel, struct list_head	*channel_list)
{
	struct list_head			*pos=NULL, *tmp=NULL;
	struct s_dsp_channel_info	*pdci=NULL;
	struct s_dsp_channel_info	*pdci_find=NULL;
	struct vocoder_stat_param	info;
	struct timeval now;
	long diff, difftime;
	float frametime, calltime;
	int voice_ctl;
	int res=-1;
	voice_ctl = open_ctl();
	if (voice_ctl < 0)
	{
		fprintf(stderr, "error: open voice_ctl '%s(%d)'", strerror(errno), errno);
		fflush(stderr);
	}
	else
	{
		info.channel = channel;
		res = do_ioctl(voice_ctl, VOCODER_STAT_CHANNEL, &info, "vocoder channel stat info");
		if (!res)
		{
			list_for_each_safe(pos, tmp, channel_list)
			{
				pdci_find = list_entry(pos, struct s_dsp_channel_info, list);
				if (pdci_find && (pdci_find->channel == channel))
				{
					pdci=pdci_find;
				}
			}
			gettimeofday(&now, NULL);			
			if (!pdci)
			{
				pdci = malloc( sizeof(struct s_dsp_channel_info) );
				if(pdci)
				{
					pdci->channel = channel;
					pdci->info = info;
					pdci->time = now;
					list_add(&(pdci->list), channel_list);
				}
			}
			
			difftime = _timevaldiff(&pdci->time, &now);
			
			fprintf(stdout, "                channel: %d\n", channel);
			/* encoder write */
			diff = info.encoder_fifo_stat.write_bytes - pdci->info.encoder_fifo_stat.write_bytes;
			fprintf(stdout, "[enc]       write bytes: %*lu(%*ld)\n", 10, info.encoder_fifo_stat.write_bytes, 10, diff);
			diff = info.encoder_fifo_stat.write_frames - pdci->info.encoder_fifo_stat.write_frames;
			fprintf(stdout, "[enc]      write frames: %*lu(%*ld)\n", 10, info.encoder_fifo_stat.write_frames, 10, diff);
			frametime = (diff > 0) ? ((float)difftime/diff) : 0;
			fprintf(stdout, "[enc]       write frame: %9.1f ms\n", frametime);
			diff = info.encoder_fifo_stat.write_calls - pdci->info.encoder_fifo_stat.write_calls;
			fprintf(stdout, "[enc]       write calls: %*lu(%*ld)\n", 10, info.encoder_fifo_stat.write_calls, 10, diff);
			calltime = (diff > 0) ? ((float)difftime/diff) : 0;
			fprintf(stdout, "[enc]        write call: %9.1f ms\n", calltime);
			/* encoder read */
			diff = info.encoder_fifo_stat.read_bytes - pdci->info.encoder_fifo_stat.read_bytes;
			fprintf(stdout, "[enc]        read bytes: %*lu(%*ld)\n", 10, info.encoder_fifo_stat.read_bytes, 10, diff);
			diff = info.encoder_fifo_stat.read_frames - pdci->info.encoder_fifo_stat.read_frames;
			fprintf(stdout, "[enc]       read frames: %*lu(%*ld)\n", 10, info.encoder_fifo_stat.read_frames, 10, diff);
			frametime = (diff > 0) ? ((float)difftime/diff) : 0;
			fprintf(stdout, "[enc]        read frame: %9.1f ms\n", frametime);
			diff = info.encoder_fifo_stat.read_calls - pdci->info.encoder_fifo_stat.read_calls;
			fprintf(stdout, "[enc]        read calls: %*lu(%*ld)\n", 10, info.encoder_fifo_stat.read_calls, 10, diff);
			calltime = (diff > 0) ? ((float)difftime/diff) : 0;
			fprintf(stdout, "[enc]         read call: %9.1f ms\n", calltime);
			fprintf(stdout, "[enc]              size: %*lu\n", 10, info.encoder_fifo_stat.size);
			/* decoder write */
			diff = info.decoder_fifo_stat.write_bytes - pdci->info.decoder_fifo_stat.write_bytes;
			fprintf(stdout, "[dec]       write bytes: %*lu(%*ld)\n", 10, info.decoder_fifo_stat.write_bytes, 10, diff);
			diff = info.decoder_fifo_stat.write_frames - pdci->info.decoder_fifo_stat.write_frames;
			fprintf(stdout, "[dec]      write frames: %*lu(%*ld)\n", 10, info.decoder_fifo_stat.write_frames, 10, diff);
			frametime = (diff > 0) ? ((float)difftime/diff) : 0;
			fprintf(stdout, "[dec]       write frame: %9.1f ms\n", frametime);
			diff = info.decoder_fifo_stat.write_calls - pdci->info.decoder_fifo_stat.write_calls;
			fprintf(stdout, "[dec]       write calls: %*lu(%*ld)\n", 10, info.decoder_fifo_stat.write_calls, 10, diff);
			calltime = (diff > 0) ? ((float)difftime/diff) : 0;
			fprintf(stdout, "[dec]        write call: %9.1f ms\n", calltime);
			/* decoder read */
			diff = info.decoder_fifo_stat.read_bytes - pdci->info.decoder_fifo_stat.read_bytes;
			fprintf(stdout, "[dec]        read bytes: %*lu(%*ld)\n", 10, info.decoder_fifo_stat.read_bytes, 10, diff);
			diff = info.decoder_fifo_stat.read_frames - pdci->info.decoder_fifo_stat.read_frames;
			fprintf(stdout, "[dec]       read frames: %*lu(%*ld)\n", 10, info.decoder_fifo_stat.read_frames, 10, diff);
			frametime = (diff > 0) ? ((float)difftime/diff) : 0;
			fprintf(stdout, "[dec]        read frame: %9.1f ms\n", frametime);
			diff = info.decoder_fifo_stat.read_calls - pdci->info.decoder_fifo_stat.read_calls;
			fprintf(stdout, "[dec]        read calls: %*lu(%*ld)\n", 10, info.decoder_fifo_stat.read_calls, 10, diff);
			calltime = (diff > 0) ? ((float)difftime/diff) : 0;
			fprintf(stdout, "[dec]         read call: %9.1f ms\n", calltime);
			fprintf(stdout, "[dec]              size: %*lu\n", 10, info.decoder_fifo_stat.size);
			
			diff = info.ofw - pdci->info.ofw;
			fprintf(stdout, "                    ofw: %*lu(%*ld)\n", 10, info.ofw, 10, diff);
			diff = info.ufw - pdci->info.ufw;
			fprintf(stdout, "                    ufw: %*lu(%*ld)\n", 10, info.ufw, 10, diff);

			fflush(stdout);
			
			if (pdci)
			{
				pdci->info = info;
				pdci->time = now;
			}
		}
		close(voice_ctl);
	}
	return res;
}
/* -------------------------------------------------------------------------- */
