#include "driver.h"

#if 0
#define DSP_DEBUG(args...) printk(args)
#else
#define DSP_DEBUG(args...)
#endif

static struct vcodec_info	info;

/*- End of function --------------------------------------------------------*/
void virtual_codec_install(dsp_t dsp)
{
	DPRINTK("DSP%d\n", dsp_get_number(dsp));
}
/*- End of function --------------------------------------------------------*/
void virtual_codec_uninstall(dsp_t dsp)
{
	dsp_free(dsp);	
}
/*- End of function --------------------------------------------------------*/
int read_channels_count(dsp_t dsp)
{
	char buf[256];

	info.ProgramVersion		= VCODEC_PROGRAM_VERSION;
	info.NumberOfChannels	= VCODEC_NUM_OF_CHANNELS;
	info.Capabilities		= VCODEC_CAPABILITIES;
	info.encoder_fifo_size	= 0;
	info.decoder_fifo_size	= 0;

	sprintf( buf, 
			 "uCode ver. %d.%d [numch=%d,caps=0x%04X] loaded", 
			 info.ProgramVersion >> 8, 
			 info.ProgramVersion & 0xFF, 
			 info.NumberOfChannels, 
			 info.Capabilities );
	DPRINTK( "%s\n", buf );
	return info.NumberOfChannels;
}
/*- End of function --------------------------------------------------------*/
void read_exchange_block(struct vocoder* v)
{
	unsigned short  Capabilities;
	
	/* copy struct to static vars */
	v->n_chns=info.NumberOfChannels;
	v->uc_code_ver=info.ProgramVersion;
	v->encoder_fifo_size=info.encoder_fifo_size;
	v->decoder_fifo_size=info.decoder_fifo_size;
	v->dtmf_timestamp=0;
	Capabilities = info.Capabilities;
	if (v->id == CODEC_ID_G711)
		Capabilities = VCODEC_G711_CAPABILITIES;
	if (v->id == CODEC_ID_LIN16)
		Capabilities = VCODEC_LIN16_CAPABILITIES;

	v->caps=Capabilities | (Capabilities << 16);

	if (info.Capabilities & CAP_MIXER) {
		v->n_inputs=0;
		v->gain_base=0xFFFF;
		v->n_mixers=v->n_chns;
	} else {
		v->n_inputs=0;
		v->gain_base=0xFFFF;
		v->n_mixers=0;
	}
}
/*- End of function --------------------------------------------------------*/
void read_channel_data(struct channel* c)
{
}
/*- End of file ------------------------------------------------------------*/
