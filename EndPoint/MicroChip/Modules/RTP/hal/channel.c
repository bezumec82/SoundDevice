#include "driver.h"

/*--------------------------------------*/
/* DSP specific functions. Not for MCC! */
/*--------------------------------------*/
void channel_init(channel_t* c)
{
	vocoder_t* v = c->parent;
	c->encoder_payloadtype = 0;
	c->decoder_payloadtype = 0;	
	switch (v->id) {
		case CODEC_ID_G711:
			c->read_enc_frame = v->read_enc_frame;
			c->read_dec_frame = v->read_dec_frame;
			c->write_enc_frame = v->write_enc_frame;
			c->write_dec_frame = v->write_dec_frame;
			c->frame_bytes = v->frame_bytes;
			c->decode_frame = v->decode_frame;
			c->encode_frame = v->encode_frame;

			c->encoder_payloadtype = PAYLOAD_TYPE_G711A;
			c->decoder_payloadtype = PAYLOAD_TYPE_G711A;
		break;
		case CODEC_ID_LIN16:
			c->read_enc_frame = v->read_enc_frame;
			c->read_dec_frame = v->read_dec_frame;
			c->write_enc_frame = v->write_enc_frame;
			c->write_dec_frame = v->write_dec_frame;
			c->frame_bytes = v->frame_bytes;
			c->decode_frame = v->decode_frame;
			c->encode_frame = v->encode_frame;
			
			c->encoder_payloadtype = PAYLOAD_TYPE_LIN16;
			c->decoder_payloadtype = PAYLOAD_TYPE_LIN16;
		case CODEC_ID_G711_MCC:
		case CODEC_ID_G723:
		case CODEC_ID_G729:
			c->read_enc_frame = v->read_enc_frame;
			c->read_dec_frame = v->read_dec_frame;
			c->write_enc_frame = v->write_enc_frame;
			c->write_dec_frame = v->write_dec_frame;
			c->frame_bytes = v->frame_bytes;
			c->decode_frame = v->decode_frame;
			c->encode_frame = v->encode_frame;

		break;
	}

	sprintf(c->devname, "%s-%d", codec_name_from_id(v->id), c->slot);
	read_channel_data(c);

	circular_buffer_init(&c->encoder_fifo, FIFO_SIZE*CHANNEL_FRAME_SIZE*sizeof(short), 0);
	circular_buffer_init(&c->decoder_fifo, FIFO_SIZE*CHANNEL_FRAME_SIZE*sizeof(short), 0);

	register_channel(c->tdm, c, v->ops);
}
/*--- End of function ----------------------------------------------------------------------------*/
void channel_deinit(channel_t* c)
{
	unregister_channel(c->tdm);
	
	circular_buffer_free(&c->encoder_fifo);
	circular_buffer_free(&c->decoder_fifo);
}
/*--- End of function ----------------------------------------------------------------------------*/
void channel_disconnect(channel_t* c)
{
	mux_disconnect(c->tdm);
}
/*--- End of function ----------------------------------------------------------------------------*/
static void lock_channel(channel_t* c, unsigned int capabilities)
{
	c->busy=1;
	c->marker=1;
	c->decoder_freeze = -1;
}
/*--- End of function ----------------------------------------------------------------------------*/

void free_channel(channel_t *c)
{
	c->busy=0;
	c->marker=0;
	return;
}
/*--- End of function ----------------------------------------------------------------------------*/
channel_t* alloc_channel(unsigned int capabilities)
{
	channel_t* c;
	vocoder_t* v;
	int i;
	DPRINTK("capabilities=%08x\n",capabilities);
	/* try to alloc preffered codec */
	for (i=TDM_DSP_START; i < TDM_MAP_SIZE; i++) {
		c = (channel_t*)tdm_map_context(i);
		if (c == 0) continue;
		if (c->busy) continue;
		v = c->parent;
		if (v->caps == capabilities)  goto done;
	}
	
	/* try to alloc optional codec */
	for (i=TDM_DSP_START; i < TDM_MAP_SIZE; i++) {
		c = (channel_t*)tdm_map_context(i);
		if (c == 0) continue;
		if (c->busy) continue;
		v = c->parent;
		if( !(capabilities & CAP_MCC) && (v->caps & CAP_MCC) )
			continue;
		if (((v->caps & capabilities) ^ capabilities) == 0) goto done;
	}
	return NULL;
done:
	lock_channel(c, capabilities);
	return c;
}
/*--- End of function ----------------------------------------------------------------------------*/

/* Get virtual channel (DSP-based) */
channel_t* get_channel(int dev_no, int chn, unsigned int capabilities)
{
	channel_t* c;
	vocoder_t* v;
	int i;
	
	if (dev_no < 0)	return NULL;
	v = vocoder_find(dev_no);
	if (v == NULL) return NULL;

	/* get specified channel for selected DSP */
	if (chn >= 0) {
		if (chn >= v->n_chns) return NULL;
		c = &v->channels[chn];
		if (c->busy == 0) goto done;
		return NULL;
	}
	
	/* get any free channel for selected DSP */
	for(i=0; i < v->n_chns; ++i) {
		c = &v->channels[i];
		if (c->busy == 0) goto done;
	}
	return NULL;
	
done:
	lock_channel(c, capabilities);
	return c;
}
/*--- End of function ----------------------------------------------------------------------------*/

/* Get virtual channel (DSP-based) */
channel_t* get_channel_tdm(int tdm, unsigned int capabilities)
{
	channel_t* c;
	vocoder_t* v;
	
	if (tdm < 0) return NULL;
	c = (channel_t*)tdm_map_context(tdm);
	if ((c == NULL) || (c->busy)) return NULL;
	v = c->parent;
	if (((v->caps & capabilities) ^ capabilities) == 0) {
		lock_channel(c, capabilities);
		return c;
	}
	return NULL;
}
/*--- End of function ----------------------------------------------------------------------------*/

char* codec_name(channel_t* c) {return c->devname;}

/*--- End of function ----------------------------------------------------------------------------*/
/*--- End of file ----------------------------*/

EXPORT_SYMBOL(get_channel_tdm);
EXPORT_SYMBOL(free_channel);
EXPORT_SYMBOL(get_channel);
EXPORT_SYMBOL(alloc_channel);
