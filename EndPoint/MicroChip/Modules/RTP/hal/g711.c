#include "driver.h"
#include "voice-ioctl.h"


#define G711_SUBFRAME_SIZE		G711_DSP_SUBFRAME_SIZE
#define G711_FRAME_SIZE			G711_DSP_FRAME_SIZE
#define AMI_MASK				0x55

unsigned char g711_table_lin2a[8192];
short g711_table_alaw[256];
/*--- End of function -----------------------------------------------------*/
static inline unsigned char linear2alaw (short int linear)
{
    int mask;
    int seg;
    int pcm_val;
    static int seg_end[8] =
    {
         0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF, 0x1FFF, 0x3FFF, 0x7FFF
    };
    
    pcm_val = linear;
    if (pcm_val >= 0)
    {
        /* Sign (7th) bit = 1 */
        mask = AMI_MASK | 0x80;
    }
    else
    {
        /* Sign bit = 0 */
        mask = AMI_MASK;
        pcm_val = -pcm_val;
    }

    /* Convert the scaled magnitude to segment number. */
    for (seg = 0;  seg < 8;  seg++)
    {
        if (pcm_val <= seg_end[seg])
	    break;
    }
    /* Combine the sign, segment, and quantization bits. */
    return  ((seg << 4) | ((pcm_val >> ((seg)  ?  (seg + 3)  :  4)) & 0x0F)) ^ mask;
}
/*--- End of function -----------------------------------------------------*/
static inline short int alaw2linear (unsigned char alaw)
{
    int i;
    int seg;

    alaw ^= AMI_MASK;
    i = ((alaw & 0x0F) << 4) + 8 /* rounding error */;
    seg = (((int) alaw & 0x70) >> 4);
    if (seg)
        i = (i + 0x100) << (seg - 1);
    return (short int) ((alaw & 0x80)  ?  i  :  -i);
}
/*--- End of function -----------------------------------------------------*/
void g711_table_init(void)
{
	int i;
	for(i = 0;i < 256;i++)
	{
		g711_table_alaw[i] = alaw2linear(i);
	}
	/* set up the reverse (mu-law) conversion table */
	for(i = -32768; i < 32768; i++)
	{
		g711_table_lin2a[((unsigned short)i) >> 3] = linear2alaw(i);
	}
}
/*--- End of function -----------------------------------------------------*/
size_t g711_read_enc_frame(struct channel* c,  int *payloadtype, void* buffer, size_t buffer_size)
{
	u8* data;
	size_t npop;
	int i, frames_count, res;
	if (payloadtype)
		*payloadtype = c->encoder_payloadtype;
		
	frames_count = (buffer_size / G711_SUBFRAME_SIZE);
	npop = 0;
	data = (u8*)buffer;
	for (i=0; i<frames_count; i++)
	{
		res = circular_buffer_pop(&c->encoder_fifo, data, G711_SUBFRAME_SIZE);
		if (res > 0)
		{
			data+=res;
			npop+=res;
			c->encoder_fifo_stat.read_bytes+=res;
			c->encoder_fifo_stat.read_frames+=1;
		}
	}
	c->encoder_fifo_stat.read_calls+=1;
	return npop;
}
/*--- End of function -----------------------------------------------------*/
size_t g711_read_dec_frame(struct channel* c,  int *payloadtype, void* buffer, size_t buffer_size)
{
	u8* data;
	size_t npop;
	int i, frames_count, res;
	if (payloadtype)
		*payloadtype = c->decoder_payloadtype;
		
	frames_count = (buffer_size / G711_SUBFRAME_SIZE);
	npop = 0;
	data = (u8*)buffer;
	for (i=0; i<frames_count; i++)
	{
		res = circular_buffer_pop(&c->decoder_fifo, data, G711_SUBFRAME_SIZE);
		if (res > 0)
		{
			data+=res;
			npop+=res;
			c->decoder_fifo_stat.read_bytes+=res;
			c->decoder_fifo_stat.read_frames+=1;
		}
	}
	c->decoder_fifo_stat.read_calls+=1;
	return npop;
}
/*--- End of function -----------------------------------------------------*/
size_t g711_write_enc_frame(struct channel* c, int payloadtype, void* buffer, size_t buffer_size)
{
	u8* 	data;
	size_t  npush=0;
	int		res;
	if ((payloadtype == PAYLOAD_TYPE_G711U) ||
		(payloadtype == PAYLOAD_TYPE_G711A))
	{
		/* no jb */
		npush = 0;
		data = (u8*)buffer;
		while(buffer_size >= G711_SUBFRAME_SIZE)
		{
			res = circular_buffer_push(&c->encoder_fifo, data, G711_SUBFRAME_SIZE);
			if (res > 0)
			{
				data += res;
				npush += res;
				c->encoder_fifo_stat.write_bytes += res;
				c->encoder_fifo_stat.write_frames += 1;
			}
			buffer_size -= G711_SUBFRAME_SIZE;
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
size_t g711_write_dec_frame(struct channel* c, int payloadtype, void* buffer, size_t buffer_size)
{
	u8* 	data;
	size_t  npush=0;
	int		res;
	if ((payloadtype == PAYLOAD_TYPE_G711U) ||
		(payloadtype == PAYLOAD_TYPE_G711A))
	{
		jb_monitor(c);
		npush = 0;
		data = (u8*)buffer;
		while(buffer_size >= G711_SUBFRAME_SIZE)
		{
			res = circular_buffer_push(&c->decoder_fifo, data, G711_SUBFRAME_SIZE);
			if (res > 0)
			{			
				data += res;		
				npush += res;
				c->decoder_fifo_stat.write_bytes += res;
				c->decoder_fifo_stat.write_frames += 1;
			}
			buffer_size -= G711_SUBFRAME_SIZE;
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
size_t g711_frame_bytes(struct channel* c)
{
	return G711_SUBFRAME_SIZE;
}
/*--- End of function -----------------------------------------------------*/
size_t g711_decode_frame(struct channel* c, void* inbuffer, size_t inbuffer_size, void* outbuffer, size_t outbuffer_size)
{
	int8_t	*src = (int8_t	*)inbuffer;
	int16_t	*dst = (int16_t	*)outbuffer;
	size_t buffer_size = (inbuffer_size > (outbuffer_size/2)) ? outbuffer_size : inbuffer_size*2;
	size_t nsamples;
	if (c && inbuffer && outbuffer && (buffer_size>0))
	{
		nsamples = buffer_size/2;
		buffer_size = nsamples*2;
		while(nsamples--)
			*dst++ = g711_table_alaw[*src++];		
	}
	return buffer_size;
}
/*--- End of function ------------------------------------------------------*/
size_t g711_encode_frame(struct channel*c, void* inbuffer, size_t inbuffer_size, void* outbuffer, size_t outbuffer_size)
{
	uint16_t	*src = (uint16_t	*)inbuffer;
	int8_t	*dst = (int8_t	*)outbuffer;
	size_t buffer_size = ((inbuffer_size/2) > outbuffer_size) ? outbuffer_size : (inbuffer_size/2);
	size_t nsamples;
	if (c && inbuffer && outbuffer && (buffer_size>0))
	{
		nsamples = buffer_size;
		while(nsamples--)
			*dst++ = g711_table_lin2a[(*src++) >> 3];
	}
	return buffer_size;
}
/*--- End of function ------------------------------------------------------*/
/*--- End of file ----------------------------------------------------------*/
