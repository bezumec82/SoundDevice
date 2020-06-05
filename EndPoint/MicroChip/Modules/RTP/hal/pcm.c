#include "driver.h"
#include "voice-ioctl.h"

#define LIN16_SUBFRAME_SIZE		LIN16_DSP_SUBFRAME_SIZE
#define LIN16_FRAME_SIZE		LIN16_DSP_FRAME_SIZE

/*--------------------------------------------------------------------------*/
size_t lin16_read_enc_frame(struct channel* c,  int *payloadtype, void* buffer, size_t buffer_size)
{
	u8* data;
	size_t npop;
	int i, res, frames_count;
	if (payloadtype)
		*payloadtype = c->encoder_payloadtype;
	
	npop = 0;
	frames_count = (buffer_size / LIN16_SUBFRAME_SIZE);
	data = (u8*)buffer;
	for (i=0; i<frames_count; i++)
	{
		res = circular_buffer_pop(&c->encoder_fifo, data, LIN16_SUBFRAME_SIZE);
		if (res > 0)
		{
			npop += res;
			data += res;
			c->encoder_fifo_stat.read_bytes += res;
			c->encoder_fifo_stat.read_frames += 1;
		}
	}
	c->encoder_fifo_stat.read_calls += 1;
	return npop;
}
/*--- End of function -----------------------------------------------------*/
size_t lin16_read_dec_frame(struct channel* c,  int *payloadtype, void* buffer, size_t buffer_size)
{
	u8* data;
	size_t npop;
	int i, res, frames_count;
	if (payloadtype)
		*payloadtype = c->decoder_payloadtype;
	
	npop = 0;
	frames_count = (buffer_size / LIN16_SUBFRAME_SIZE);
	data = (u8*)buffer;
	for (i=0; i<frames_count; i++)
	{
		res=circular_buffer_pop(&c->decoder_fifo, data, LIN16_SUBFRAME_SIZE);
		if (res > 0)
		{
			npop += res;
			data += res;
			c->decoder_fifo_stat.read_bytes += res;
			c->decoder_fifo_stat.read_frames += 1;
		}
	}
	c->decoder_fifo_stat.read_calls += 1;
	return npop;
}
/*--- End of function -----------------------------------------------------*/
size_t lin16_write_enc_frame(struct channel* c, int payloadtype, void* buffer, size_t buffer_size)
{
	u8* 	data;
	size_t  npush=0;
	int 	res;
	if (payloadtype == PAYLOAD_TYPE_LIN16)
	{
		/* no jb */
		npush = 0;
		data = (u8*)buffer;
		while(buffer_size >= LIN16_SUBFRAME_SIZE)
		{
			res = circular_buffer_push(&c->encoder_fifo, data, LIN16_SUBFRAME_SIZE);
			if (res > 0)
			{
				data += res;
				npush += res;
				c->encoder_fifo_stat.write_bytes += res;
				c->encoder_fifo_stat.write_frames += 1;
			}
			buffer_size -= LIN16_SUBFRAME_SIZE;
		}
		c->encoder_fifo_stat.write_calls += 1;
	}
	else
	{
		DPRINTK("wrong pt=%d\n", payloadtype);
	}
	return npush;
}
/*--- End of function -----------------------------------------------------*/
size_t lin16_write_dec_frame(struct channel* c, int payloadtype, void* buffer, size_t buffer_size)
{
	u8* 	data;
	size_t  npush=0;
	int 	res;
	if (payloadtype == PAYLOAD_TYPE_LIN16)
	{
		jb_monitor(c);
		npush = 0;
		data = (u8*)buffer;
		while(buffer_size >= LIN16_SUBFRAME_SIZE)
		{
			res = circular_buffer_push(&c->decoder_fifo, data, LIN16_SUBFRAME_SIZE);
			if (res > 0)
			{
				data += res;
				npush += res;
				c->decoder_fifo_stat.write_bytes += res;
				c->decoder_fifo_stat.write_frames += 1;
			}
			
			buffer_size -= LIN16_SUBFRAME_SIZE;
		}
		c->decoder_fifo_stat.write_calls += 1;		
	}
	else
	{
		DPRINTK("wrong pt=%d\n", payloadtype);
	}
	return npush;
}
/*--- End of function -----------------------------------------------------*/
size_t lin16_frame_bytes(struct channel* c)
{
	return LIN16_SUBFRAME_SIZE;
};
/*--- End of function -----------------------------------------------------*/
size_t lin16_decode_frame(struct channel* c, void* inbuffer, size_t inbuffer_size, void* outbuffer, size_t outbuffer_size)
{
	size_t buffer_size = (inbuffer_size > outbuffer_size) ? outbuffer_size : inbuffer_size;
	if (c && inbuffer && outbuffer && (buffer_size>0))
	{
		memcpy(outbuffer, inbuffer, buffer_size);
	}
	return buffer_size;
}
/*--- End of function ------------------------------------------------------*/
size_t lin16_encode_frame(struct channel*c, void* inbuffer, size_t inbuffer_size, void* outbuffer, size_t outbuffer_size)
{
	return lin16_decode_frame(c, inbuffer, inbuffer_size, outbuffer, outbuffer_size);
}
/*--- End of function ------------------------------------------------------*/
/*--- End of file ---------------------------------------------------------*/
