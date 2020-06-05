/*
 * simple char interface to Blackfin SPORT peripheral
 *
 * Copyright 2004-2010 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 * 
 * Pwnd by q2l.1
 * 
 */

#include <linux/cdev.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/proc_fs.h>

#include "bf5xx_sport_ex.h"
#include "spi_tdm.h"
#include "clk-common.h"


#define UPDATE_MAX_SAMPLES
#define SPORT_DRVNAME		"bfin_sport_ex"

#ifdef DEBUG
#define DPRINTK(fmt, args...) printk("%s: " fmt, __FUNCTION__ , ## args)
#else
#define DPRINTK(fmt, args...)
#endif

int spi_tdm_demux(int channels_count, int channel_size, void *pdemux);
int spi_tdm_mux(int channels_count, int channel_size, void *pmux);

struct sport_stream_config_profile sport_stream_config_list[]=
{
	{
		.freq = 8000,
		.frame_size = (FRAME_SIZE*SPORT_PACKET_FACTOR),
		.config = {
			.rx_len = ((FRAME_SIZE*SPORT_PACKET_FACTOR)*CHANNELS_MAX*6)/BFIN_SUBBUFFER_COUNT,
			.count_rx_sub = 6,
			.delay_rx_sub = 3,/*2*/
			.read_buf_len = (FRAME_SIZE*SPORT_PACKET_FACTOR)*CHANNELS_MAX,
	
			.tx_len = ((FRAME_SIZE*SPORT_PACKET_FACTOR)*CHANNELS_MAX*6)/BFIN_SUBBUFFER_COUNT,
			.count_tx_sub = 6,
			.delay_tx_sub = 5,/*2*/
			.write_buf_len = (FRAME_SIZE*SPORT_PACKET_FACTOR)*CHANNELS_MAX
		}
	},
	{
		.freq = 16000,
		.frame_size = (FRAME16K_SIZE*SPORT_PACKET_FACTOR),
		.config = {
			.rx_len = ((FRAME16K_SIZE*SPORT_PACKET_FACTOR)*CHANNELS_MAX*6)/BFIN_SUBBUFFER_COUNT,
			.count_rx_sub = 6,
			.delay_rx_sub = 3,/*2*/
			.read_buf_len = (FRAME16K_SIZE*SPORT_PACKET_FACTOR)*CHANNELS_MAX,
	
			.tx_len = ((FRAME16K_SIZE*SPORT_PACKET_FACTOR)*CHANNELS_MAX*6)/BFIN_SUBBUFFER_COUNT,
			.count_tx_sub = 6,
			.delay_tx_sub = 5,/*2*/
			.write_buf_len = (FRAME16K_SIZE*SPORT_PACKET_FACTOR)*CHANNELS_MAX
		}
	},
};

struct sport_config sport_config_list[]=
{
	{
		.mode				= TDM_MODE,
		.frame_delay 		= 1,	/* Delay between frame sync pulse and first bit */
		.right_first		= 0,	/* Right stereo channel first */

		.lsb_first			= 0,	/* order of transmit or receive data */
		.fsync				= 1,	/* Frame sync required */
		.data_indep			= 0,	/* data independent frame sync generated */
		.act_low			= 0,	/* Active low TFS */
		.late_fsync			= 0,	/* Late frame sync */
		.tckfe				= 0,
		.sec_en				= 0,	/* Secondary side enabled */

		.data_format		= NORM_FORMAT,	/* Normal, u-law or a-law */
		.word_len			= 16,			/* How length of the word in bits, 3-32 bits */
		.dma_enabled		= 1,		
		
		.channels			= 8,
		.int_clk			= 0,
		.serial_clk			= 0,
		.fsync_clk			= 8000,
	},
	{
		.mode				= TDM_MODE,
		.frame_delay 		= 1,	/* Delay between frame sync pulse and first bit */
		.right_first		= 0,	/* Right stereo channel first */

		.lsb_first			= 0,	/* order of transmit or receive data */
		.fsync				= 1,	/* Frame sync required */
		.data_indep			= 0,	/* data independent frame sync generated */
		.act_low			= 0,	/* Active low TFS */
		.late_fsync			= 0,	/* Late frame sync */
		.tckfe				= 0,
		.sec_en				= 0,	/* Secondary side enabled */

		.data_format		= NORM_FORMAT,	/* Normal, u-law or a-law */
		.word_len			= 16,			/* How length of the word in bits, 3-32 bits */
		.dma_enabled		= 1,		
		
		.channels			= 8,
		.int_clk			= 0,
		.serial_clk			= 0,
		.fsync_clk			= 16000,
	}
};
/* -------------------------------------------------------------------------- */

struct sport_dev {
	int	id;
	int in_use;
	int dma_rx_chan;
	int dma_tx_chan;

	int rx_irq;
	unsigned char 	*rx_buf;	/* Buffer store the received data */
	unsigned char 	*rx_buf_int;
	long			*rx_max;
	int				rx_max_len;	

	int rx_len;		/* How many bytes will be received */
	int rx_sub_len;
	int rx_received;	/* How many bytes has been received */
	int rx_int_signal;
	unsigned long rx_total;
	unsigned long rx_int_total;

	int tx_irq;
	unsigned char	*tx_buf;
	unsigned char	*tx_buf_int;
	long			*tx_max;
	int				tx_max_len;

	int tx_len;
	int tx_buf_len;
	int tx_buf_int_len;
	int tx_sub_len;	
	int tx_sent;
	int tx_int_signal;
	unsigned long tx_int_total;


	int	count_rx_sub;
	int	count_tx_sub;

	int	delay_rx_sub;
	int	delay_tx_sub;
	
	struct dmasg *dma_rx_desc;
	struct dmasg *dma_tx_desc;
	struct dmasg *curr_rx_desc;
	struct dmasg *curr_tx_desc;

	int write_buf_len;
	int read_buf_len;

	int err_irq;

	int sport_clkdiv;

	struct completion c;

	volatile struct sport_register *regs;
	struct sport_config config;
	
	struct tasklet_struct rx_tsk;
	struct tasklet_struct tx_tsk;
	
	struct s_sysclk_config	sysclk_config;
	int						sysclk_id;
};

/* XXX: this should get pushed to platform device */
#define SPORT_REQ(x) \
	       P_SPORT##x##_TFS, P_SPORT##x##_DTPRI, P_SPORT##x##_TSCLK, P_SPORT##x##_DTSEC, \
	       P_SPORT##x##_RFS, P_SPORT##x##_DRPRI, P_SPORT##x##_RSCLK, P_SPORT##x##_DRSEC
static u16 sport_req[9] = {
#ifdef SPORT1_TCR1
	SPORT_REQ(1)
#endif
};

#define SPORT_PARAMS(x) \
		.id          = x, \
		.in_use      = 0, \
		.dma_rx_chan = CH_SPORT##x##_RX, \
		.dma_tx_chan = CH_SPORT##x##_TX, \
		.rx_irq      = IRQ_SPORT##x##_RX, \
		.tx_irq      = IRQ_SPORT##x##_TX, \
		.err_irq     = IRQ_SPORT##x##_ERROR, \
		.regs        = (void *)SPORT##x##_TCR1, \

static struct sport_dev sport_device = {
#ifdef SPORT1_TCR1
	SPORT_PARAMS(1)
#endif
	.sysclk_config	= 
	{
		.normal_clock			= 8000,
		.normal_clock_low		= 7800,
		.normal_clock_high		= 8200,
		.clock_fail_threshold	= 3,
		.clock_normal_threshold	= 2,
		.tick_inc				= 320,
		.ticks_per_ms			= (8000 * sizeof(short) * CHANNELS_MAX)/1000
	},

	.sysclk_id					= -1
};

static irqreturn_t sport_rx_handler(int irq, void *dev_id);
static irqreturn_t sport_tx_handler(int irq, void *dev_id);

static void rx_tsk_process(unsigned long data);
static void tx_tsk_process(unsigned long data);

static inline int sport_start(struct sport_dev *dev);
static inline int sport_stop(struct sport_dev *dev);

static inline int sport_rx_dma_init(struct sport_dev *dev);
static inline int sport_tx_dma_init(struct sport_dev *dev);

static ssize_t sport_read(struct sport_dev *dev);
static ssize_t sport_write(struct sport_dev *dev);
static int sport_release(struct sport_dev *dev);
static int sport_open(struct sport_dev *dev);
/* -------------------------------------------------------------------------- */
#ifdef UPDATE_MAX_SAMPLES
static int update_sport_max(struct sport_dev *dev, int rxtx)
{
	short 		*pbasebuf = NULL;
	short 		*pbuf = NULL;
	size_t 		count = 0;
	long 		*pbasemaxbuf = NULL;
	long 		*pmaxbuf = NULL;
	size_t 		max_count = 0;
	int 		i, j;
	
	if (dev)
	{
		if (rxtx)
		{
			pbasebuf = (short *)dev->rx_buf_int;
			count = dev->rx_received / 2;
			pbasemaxbuf = dev->rx_max;
			max_count = dev->rx_max_len;
		}
		else
		{
			pbasebuf = (short *)dev->tx_buf_int;
			count = dev->tx_sent / 2;
			pbasemaxbuf = dev->tx_max;
			max_count = dev->tx_max_len;
		}
		
		if (pbasebuf && pbasemaxbuf)
		{
			for (j=0;j<max_count;j++)
			{
				pbuf = &pbasebuf[j];
				pmaxbuf = &pbasemaxbuf[j];
				for (i=j;i<count;i+=max_count)
				{				
					if (*pmaxbuf < *pbuf)
						*pmaxbuf = *pbuf;
					pbuf += max_count;
				}
			}
		}
	}
	
	return 0;
}
#endif
/* -------------------------------------------------------------------------- */
/* note: multichannel is in units of 8 channels, 
 * tdm_count is # channels NOT / 8 ! */
static int sport_set_multichannel(volatile struct sport_register *regs,
				  int tdm_count, int packed, int frame_delay)
{
	if (tdm_count) {
		int shift = 32 - tdm_count;
		unsigned int mask = (0xffffffff >> shift);

		regs->mcmc1 = ((tdm_count >> 3) - 1) << 12;	/* set WSIZE bits */
		regs->mcmc2 = (frame_delay << 12) | MCMEN |
		    (packed ? (MCDTXPE | MCDRXPE) : 0);

		regs->mtcs0 = regs->mrcs0 = mask;
	} else {
		regs->mcmc1 = regs->mcmc2 = 0;
		regs->mtcs0 = regs->mrcs0 = 0;
	}

	regs->mtcs1 = regs->mtcs2 = regs->mtcs3 = 0;
	regs->mrcs1 = regs->mrcs2 = regs->mrcs3 = 0;

	SSYNC();

	return 0;
}
/* -------------------------------------------------------------------------- */
static int dma_free_tx_buf(struct sport_dev *dev)
{
	if (dev)
	{
		if (dev->tx_buf)
		{
			dma_free_coherent(NULL, dev->tx_buf_len, dev->tx_buf, 0);
			dev->tx_buf = NULL;
		}
		
		if (dev->tx_buf_int)
		{
			dma_free_coherent(NULL, dev->tx_buf_int_len, dev->tx_buf_int, 0);
			dev->tx_buf_int = NULL;
		}		
		
		if (dev->dma_tx_desc)
		{
			dma_free_coherent(NULL, dev->count_tx_sub*sizeof(struct dmasg), dev->dma_tx_desc, 0);
			dev->count_tx_sub = 0;
			dev->dma_tx_desc = NULL;
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int dma_free_rx_buf(struct sport_dev *dev)
{
	if (dev)
	{
		if (dev->rx_buf)
		{
			dma_free_coherent(NULL, dev->rx_len, dev->rx_buf, 0);
			dev->rx_buf = NULL;
		}
		
		if (dev->rx_buf_int)
		{
			dma_free_coherent(NULL, dev->rx_len, dev->rx_buf_int, 0);
			dev->rx_buf_int = NULL;
		}		
		
		if (dev->dma_rx_desc)
		{
			dma_free_coherent(NULL, dev->count_rx_sub*sizeof(struct dmasg), dev->dma_rx_desc, 0);			
			dev->count_rx_sub = 0;
			dev->dma_rx_desc = NULL;
		}		
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int sport_stream_configure(struct sport_dev *dev, struct sport_stream_config *stream_config)
{
	dma_addr_t addr;

	DPRINTK("sport stream configure: rx_len=0x%08X, tx_len=0x%08X\n", stream_config->rx_len, stream_config->tx_len);
	if ((dev->rx_buf == NULL) || ((dev->rx_buf != NULL) && (dev->rx_len != stream_config->rx_len)))	
	{
		dma_free_rx_buf(dev);
		
		if (stream_config->rx_len > 0)
		{	
			dev->rx_len = stream_config->rx_len;
			dev->read_buf_len = (stream_config->read_buf_len > dev->rx_len) ? dev->rx_len : stream_config->read_buf_len;
			dev->count_rx_sub = (stream_config->count_rx_sub <= 0) ? AUTOBUFFER_COUNT : stream_config->count_rx_sub;
			dev->delay_rx_sub = (stream_config->delay_rx_sub <= 0) ? AUTOBUFFER_DELAY : stream_config->delay_rx_sub;
			
			dev->rx_sub_len = dev->rx_len/dev->count_rx_sub;
			dev->rx_buf = dma_alloc_coherent(NULL, dev->rx_len, &addr, 0);		
			dev->rx_buf_int = dma_alloc_coherent(NULL, dev->rx_len, &addr, 0);
			dev->sysclk_config.tick_inc = dev->rx_sub_len;			

			dev->dma_rx_desc =  dma_alloc_coherent(NULL, dev->count_rx_sub*sizeof(struct dmasg), &addr, 0);
			dev->curr_rx_desc = dev->dma_rx_desc;
		}
	}
	
	if ((dev->tx_buf == NULL) || ((dev->tx_buf != NULL) && (dev->tx_len != stream_config->tx_len)))	
	{
		dma_free_tx_buf(dev);
				
		if (stream_config->tx_len > 0)
		{	
			dev->tx_len = stream_config->tx_len;
			dev->tx_buf_len = dev->tx_len;
			dev->tx_buf_int_len = (dev->tx_len<<1);

			dev->write_buf_len = (stream_config->write_buf_len > dev->tx_len) ? dev->tx_len : stream_config->write_buf_len;
			dev->count_tx_sub = (stream_config->count_tx_sub <= 0) ? AUTOBUFFER_COUNT : stream_config->count_tx_sub;
			dev->delay_tx_sub = (stream_config->delay_tx_sub <= 0) ? AUTOBUFFER_DELAY : stream_config->delay_tx_sub;		
			dev->tx_sub_len = dev->tx_len/dev->count_tx_sub;			
			
			dev->tx_buf = dma_alloc_coherent(NULL, dev->tx_buf_len, &addr, 0);
			dev->tx_buf_int = dma_alloc_coherent(NULL, dev->tx_buf_int_len, &addr, 0);			
			memset(dev->tx_buf, 0, dev->tx_buf_len);
			memset(dev->tx_buf_int, 0, dev->tx_buf_int_len);
			
			dev->dma_tx_desc =  dma_alloc_coherent(NULL, dev->count_tx_sub*sizeof(struct dmasg), &addr, 0);			
			dev->curr_tx_desc = dev->dma_tx_desc;
		}
	}	
	
	return 0;	
}
/* -------------------------------------------------------------------------- */
static int sport_configure(struct sport_dev *dev, struct sport_config *config)
{
	int 	ret;
	unsigned int tcr1, tcr2, rcr1, rcr2;
	unsigned int clkdiv, fsdiv;
	struct sport_config *old_cfg = &dev->config;
	u_long sclk;
	
	tcr1 = tcr2 = rcr1 = rcr2 = 0;
	clkdiv = fsdiv = 0;

	memcpy(old_cfg, config, sizeof(*config));

	if ((dev->regs->tcr1 & TSPEN) || (dev->regs->rcr1 & RSPEN))
	{
		pr_err("channels busy\n");
		return -EBUSY;
	}

	if (config->mode == TDM_MODE) {
		if (config->channels & 0x7 || config->channels > 32)
		{
			pr_err("channels value fail\n");
			return -EINVAL;
		}
		
#ifdef UPDATE_MAX_SAMPLES		
		/*  init rx max buff */
		if ((dev->rx_max_len != config->channels) || !dev->rx_max)
		{
			if (dev->rx_max)
			{
				kfree(dev->rx_max);
				dev->rx_max = NULL;
				dev->rx_max_len = 0;
			}
			dev->rx_max_len = config->channels;
			dev->rx_max = kmalloc(dev->rx_max_len * sizeof(long), GFP_KERNEL);
			if (dev->rx_max)
			{
				memset(dev->rx_max, 0, dev->rx_max_len * sizeof(long));
			}
		}
		
		/*  init tx max buff */
		if ((dev->tx_max_len != config->channels) || !dev->tx_max)
		{
			if (dev->tx_max)
			{
				kfree(dev->tx_max);
				dev->tx_max = NULL;
				dev->tx_max_len = 0;
			}
			dev->tx_max_len = config->channels;
			dev->tx_max = kmalloc(dev->tx_max_len * sizeof(long), GFP_KERNEL);
			if (dev->tx_max)
			{
				memset(dev->tx_max, 0, dev->tx_max_len * sizeof(long));
			}
		}
#endif
		sport_set_multichannel(dev->regs, config->channels, 1,
				       config->frame_delay);
				       
		tcr1 |= (config->lsb_first << 4) | (config->fsync << 10) |
		      (config->data_indep << 11) | (config->act_low << 12) |
		      (config->late_fsync << 13) | (config->tckfe << 14);

		rcr1 |= (config->lsb_first << 4) | (config->fsync << 10) |
		      (config->data_indep << 11) | (config->act_low << 12) |
		      (config->late_fsync << 13) | (config->tckfe << 14);
		
	} else if (config->mode == I2S_MODE) {
		tcr1 |= (TCKFE | TFSR);
		tcr2 |= TSFSE;

		rcr1 |= (RCKFE | RFSR);
		rcr2 |= RSFSE;
	} else {
		tcr1 |= (config->lsb_first << 4) | (config->fsync << 10) |
		      (config->data_indep << 11) | (config->act_low << 12) |
		      (config->late_fsync << 13) | (config->tckfe << 14);
		if (config->sec_en)
			tcr2 |= TXSE;

		rcr1 |= (config->lsb_first << 4) | (config->fsync << 10) |
		      (config->data_indep << 11) | (config->act_low << 12) |
		      (config->late_fsync << 13) | (config->tckfe << 14);
		if (config->sec_en)
			rcr2 |= RXSE;
	}

	/* Using internal clock */
	if (config->int_clk) {
		sclk = get_sclk();

		if (config->serial_clk < 0 || config->serial_clk > sclk / 2)
		{
			pr_err("serial_clk value fail\n");
			return -EINVAL;
		}
		clkdiv = sclk / (2 * config->serial_clk) - 1;
		fsdiv = (config->fsync_clk ? config->serial_clk / config->fsync_clk - 1 : 0);

		tcr1 |= (ITCLK | ITFS);
		rcr1 |= (IRCLK | IRFS);
	}


	/* Setting data format */
	tcr1 |= (config->data_format << 2);	/* Bit TDTYPE */
	rcr1 |= (config->data_format << 2);	/* Bit TDTYPE */
	if (config->word_len >= 3 && config->word_len <= 32) {
		tcr2 |= config->word_len - 1;
		rcr2 |= config->word_len - 1;
	} else
	{
		pr_err("word lenght value fail\n");
		return -EINVAL;
	}

	dev->regs->rcr1 = rcr1;
	dev->regs->rcr2 = rcr2;
	dev->regs->rclkdiv = clkdiv;
	dev->regs->rfsdiv = fsdiv;
	dev->regs->tcr1 = tcr1;
	dev->regs->tcr2 = tcr2;
	dev->regs->tclkdiv = clkdiv;
	dev->regs->tfsdiv = fsdiv;
	SSYNC();
	
	if (config->dma_enabled)
	{
		sport_rx_dma_init(dev);
		sport_tx_dma_init(dev);
		sport_start(dev);
	}

	ret = request_irq(dev->rx_irq, sport_rx_handler, IRQF_DISABLED, SPORT_DRVNAME "-rx", dev);
	if (ret) {
		pr_err("unable to request sport rx irq\n");		
	}	

	ret = request_irq(dev->tx_irq, sport_tx_handler, IRQF_DISABLED, SPORT_DRVNAME "-tx", dev);
	if (ret) {
		pr_err("unable to request sport tx irq\n");		
	}	


	/*pr_err("clkdiv = %d, fsdiv = %d, sclk = %ld ", clkdiv, fsdiv, sclk);*/
#if 0
 	struct dma_register *rx_regs, *tx_regs;
	pr_debug("tcr1:0x%x, tcr2:0x%x, rcr1:0x%x, rcr2:0x%x\n"
		 "mcmc1:0x%x, mcmc2:0x%x, mtcs0:0x%x, mrcs0:0x%x\n",
		 dev->regs->tcr1, dev->regs->tcr2,
		 dev->regs->rcr1, dev->regs->rcr2,
		 dev->regs->mcmc1, dev->regs->mcmc2,
		 dev->regs->mtcs0, dev->regs->mrcs0);
		 
	rx_regs = dma_ch[dev->dma_rx_chan].regs;
	pr_debug("dma rx:\n");
	pr_debug("start_addr : 0x%08X,   buf addr : 0x%08X\n "
			 "cfg : 0x%08X,  irq_status : 0x%08X\n"	
			 "x_count : 0x%x, x_modify : 0x%x, y_count : 0x%x, y_modify : 0x%x \n"
			 ,
				rx_regs->start_addr, dev->rx_buf,
				rx_regs->cfg, rx_regs->irq_status,
				rx_regs->x_count, rx_regs->x_modify,
				rx_regs->y_count, rx_regs->y_modify);
				
	tx_regs = dma_ch[dev->dma_tx_chan].regs;
	pr_debug("dma tx:\n");
	pr_debug("start_addr : 0x%x,   buf addr : 0x%x\n "
			 "cfg : 0x%x,  irq_status : 0x%x\n"	
			 "x_count : 0x%x, x_modify : 0x%x, y_count : 0x%x, y_modify : 0x%x \n"
			 ,
				tx_regs->start_addr, dev->tx_buf,
				tx_regs->cfg, tx_regs->irq_status,
				tx_regs->x_count, tx_regs->x_modify,
				tx_regs->y_count, tx_regs->y_modify);				
#endif
	return 0;
}
/* -------------------------------------------------------------------------- */
static inline uint16_t sport_wordsize(int word_len)
{
	uint16_t wordsize = 0;

	if (word_len <= 8)
		wordsize = WDSIZE_8;
	else if (word_len <= 16)
		wordsize = WDSIZE_16;
	else if (word_len <= 32)
		wordsize = WDSIZE_32;
	else
		pr_err("word_len of %d is invalid\n", word_len);

	return wordsize;
}
/* -------------------------------------------------------------------------- */
static inline uint16_t sport_wordsize_in_bytes(int word_len)
{
	uint16_t wordsize = 0;

	if (word_len <= 8)
		wordsize = 1;
	else if (word_len <= 16)
		wordsize = 2;
	else if (word_len <= 32)
		wordsize = 4;

	return wordsize;
}
/* -------------------------------------------------------------------------- */
static inline int sport_start(struct sport_dev *dev)
{
	if (dev->config.dma_enabled)
	{
		enable_dma(dev->dma_rx_chan);
		enable_dma(dev->dma_tx_chan);
	}
	dev->regs->rcr1 |= RSPEN;
	dev->regs->tcr1 |= TSPEN;
	SSYNC();

	return 0;
}
/* -------------------------------------------------------------------------- */
static inline int sport_stop(struct sport_dev *dev)
{
	dev->regs->tcr1 &= ~TSPEN;
	dev->regs->rcr1 &= ~RSPEN;
	SSYNC();
	if (dev->config.dma_enabled)
	{
		disable_dma(dev->dma_rx_chan);
		disable_dma(dev->dma_tx_chan);
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static void setup_desc(struct dmasg *desc, void *buf, int fragcount,
		size_t fragsize, unsigned int cfg,
		unsigned int x_count, unsigned int ycount, size_t wdsize)
{

	int i;

	for (i = 0; i < fragcount; ++i) {
		desc[i].next_desc_addr  = &(desc[i + 1]);
		desc[i].start_addr = (unsigned long)buf + i*fragsize;
		desc[i].cfg = cfg;
		desc[i].x_count = x_count;
		desc[i].x_modify = wdsize;
		desc[i].y_count = ycount;
		desc[i].y_modify = wdsize;
	}

	/* make circular */
	desc[fragcount-1].next_desc_addr = desc;
}
/* -------------------------------------------------------------------------- */
static inline int sport_rx_dma_init(struct sport_dev *dev)
{
	unsigned int x_count;
	unsigned int y_count;
	unsigned int dma_cfg;
	unsigned int fragsize, fragcount;

	struct sport_config *cfg = &dev->config;
	int word_bytes = (cfg->word_len + 7) / 8;
	
	
	if (word_bytes == 3)
		word_bytes = 4;

	fragcount	= dev->count_rx_sub;
	fragsize	= dev->rx_len/fragcount;

 	x_count 	= fragsize / word_bytes;
 	y_count	= 0;
	
	dma_cfg 	= (WNR | sport_wordsize(cfg->word_len) | DMAFLOW_LARGE | NDSIZE_9 | DI_EN | DMAEN);
	
	if (y_count != 0)
		dma_cfg |= DMA2D;

	setup_desc(dev->dma_rx_desc, dev->rx_buf, fragcount, fragsize,
			dma_cfg, x_count, y_count, word_bytes);
			
	dev->curr_rx_desc = dev->dma_rx_desc;
	set_dma_next_desc_addr(dev->dma_rx_chan, dev->curr_rx_desc);
	set_dma_x_count(dev->dma_rx_chan, 0);
	set_dma_x_modify(dev->dma_rx_chan, word_bytes);
	set_dma_y_count(dev->dma_rx_chan, 0);
	set_dma_y_modify(dev->dma_rx_chan, word_bytes);	
	set_dma_config(dev->dma_rx_chan, dma_cfg);
	set_dma_curr_addr(dev->dma_rx_chan, dev->curr_rx_desc->start_addr);
	
	SSYNC();				
	
	return 0;
}
/* -------------------------------------------------------------------------- */
static inline int sport_tx_dma_init(struct sport_dev *dev)
{
	unsigned int x_count;
	unsigned int y_count;
	unsigned int dma_cfg;
	unsigned int fragsize, fragcount;

	struct sport_config *cfg = &dev->config;
	int word_bytes = (cfg->word_len + 7) / 8;
	
	
	if (word_bytes == 3)
		word_bytes = 4;

	fragcount	= dev->count_tx_sub;
	fragsize	= dev->tx_len/fragcount;

 	x_count 	= fragsize / word_bytes;
 	y_count	= 0;
	
	dma_cfg 	= (sport_wordsize(cfg->word_len) | DMAFLOW_LARGE | NDSIZE_9 | DI_EN | DMAEN);
	
	if (y_count != 0)
		dma_cfg |= DMA2D;

	setup_desc(dev->dma_tx_desc, dev->tx_buf, fragcount, fragsize,
			dma_cfg, x_count, y_count, word_bytes);
			
	dev->curr_tx_desc = dev->dma_tx_desc;
	set_dma_next_desc_addr(dev->dma_tx_chan, dev->curr_tx_desc);
	set_dma_x_count(dev->dma_tx_chan, 0);
	set_dma_x_modify(dev->dma_tx_chan, word_bytes);
	set_dma_y_count(dev->dma_tx_chan, 0);
	set_dma_y_modify(dev->dma_tx_chan, word_bytes);		
	set_dma_config(dev->dma_tx_chan, dma_cfg);
	set_dma_curr_addr(dev->dma_tx_chan, dev->curr_tx_desc->start_addr);
	
	SSYNC();				
	
	return 0;
}
/* -------------------------------------------------------------------------- */
static inline int get_dma_curr_rx_desc_index(struct sport_dev *dev)
{
	unsigned long desc, curr_desc;
	int index = -1;
	
	desc = (unsigned long)dev->dma_rx_desc;
	curr_desc = (unsigned long)get_dma_curr_desc_ptr(dev->dma_rx_chan);

	index = (curr_desc - desc)/sizeof(struct dmasg);
	if (index >= dev->count_rx_sub) index -= dev->count_rx_sub;
	return index;
}
/* -------------------------------------------------------------------------- */
static inline int get_dma_curr_tx_desc_index(struct sport_dev *dev)
{
	unsigned long desc, curr_desc;
	int index = -1;
	
	desc = (unsigned long)dev->dma_tx_desc;
	curr_desc = (unsigned long)get_dma_curr_desc_ptr(dev->dma_tx_chan);

	index = (curr_desc - desc)/sizeof(struct dmasg);
	if (index >= dev->count_tx_sub) index -= dev->count_tx_sub;
	return index;
}
/* -------------------------------------------------------------------------- */
static inline unsigned char *get_curr_rx_buf(struct sport_dev *dev)
{
	unsigned char *rx_buf=NULL;
	int index;
	index = get_dma_curr_rx_desc_index(dev);
	
	index = (index - dev->delay_rx_sub + dev->count_rx_sub);
	if (index >= dev->count_rx_sub) index -= dev->count_rx_sub;
	
	rx_buf = (unsigned char *)dev->dma_rx_desc[index].start_addr;	
	
	return rx_buf;
}
/* -------------------------------------------------------------------------- */
static inline unsigned char *get_curr_tx_buf(struct sport_dev *dev)
{
	unsigned char *tx_buf=NULL;
	int index;
	index = get_dma_curr_tx_desc_index(dev);
	
	index = (index - dev->delay_tx_sub + dev->count_tx_sub);
	if (index >= dev->count_tx_sub) index -= dev->count_tx_sub;
	
	tx_buf = (unsigned char *)dev->dma_tx_desc[index].start_addr;	
	
	return tx_buf;
}
/* -------------------------------------------------------------------------- */
static irqreturn_t sport_rx_handler(int irq, void *dev_id)
{
	unsigned char *rx_buf;
	struct sport_dev *dev = dev_id;	
	int nmove;

	clear_dma_irqstat(dev->dma_rx_chan);
	SSYNC();

	rx_buf = get_curr_rx_buf(dev);
	if (rx_buf)
	{
		if ((dev->rx_received + dev->rx_sub_len) > dev->rx_len)
		{
			nmove = (dev->rx_received + dev->rx_sub_len) - dev->rx_len;
			dev->rx_received -= nmove;
			memmove(dev->rx_buf_int, &dev->rx_buf_int[nmove], dev->rx_received);
		}
		memcpy(&dev->rx_buf_int[dev->rx_received], rx_buf, dev->rx_sub_len);
		dev->rx_received += dev->rx_sub_len;
	}

	if (dev->rx_received >= dev->read_buf_len)
	{
		if (dev->rx_int_signal == 0)
		{
			dev->rx_int_signal = 1;
		}
	}
	tasklet_schedule(&dev->rx_tsk);
	dev->rx_total += dev->rx_sub_len;
	dev->rx_int_total++;
	if (dev->sysclk_id >= 0)
		sysclk_source_tick(dev->sysclk_id, dev->rx_total);		

	return IRQ_HANDLED;
}
/* -------------------------------------------------------------------------- */
static irqreturn_t sport_tx_handler(int irq, void *dev_id)
{
	unsigned char *tx_buf;			
	struct sport_dev *dev = dev_id;
	
	
	clear_dma_irqstat(dev->dma_tx_chan);
	SSYNC();
	
	tx_buf = get_curr_tx_buf(dev);
	if (tx_buf)
	{
		if (dev->tx_sent >= dev->tx_sub_len)
		{
			memcpy(tx_buf, dev->tx_buf_int, dev->tx_sub_len);
			dev->tx_sent -= dev->tx_sub_len;
			memmove(dev->tx_buf_int, &dev->tx_buf_int[dev->tx_sub_len], dev->tx_sent);
		}
		else
		{
			memset(tx_buf, 0, dev->tx_sub_len);	
			memcpy(tx_buf, dev->tx_buf_int, dev->tx_sent);
			dev->tx_sent = 0;
		}
	}
	
	tasklet_schedule(&dev->tx_tsk);
	
	dev->tx_int_total++;

	return IRQ_HANDLED;
}
/* -------------------------------------------------------------------------- */

static irqreturn_t sport_err_handler(int irq, void *dev_id)
{
	struct sport_dev *dev = dev_id;
	uint16_t status;

	DPRINTK("%s enter\n", __func__);
	status = dev->regs->stat;

	if (status & (TOVF | TUVF | ROVF | RUVF)) {
		dev->regs->stat = (status & (TOVF | TUVF | ROVF | RUVF));
		
		sport_stop(dev);

		if (!dev->config.dma_enabled && !dev->config.int_clk) {
			if (status & TUVF)
				complete(&dev->c);
		} else
			pr_warning("sport %p status error:%s%s%s%s\n",
			       dev->regs,
			       status & TOVF ? " TOVF" : "",
			       status & TUVF ? " TUVF" : "",
			       status & ROVF ? " ROVF" : "",
			       status & RUVF ? " RUVF" : "");
	}

	/* XXX: should we always complete here and have read/write error ? */

	return IRQ_HANDLED;
}
/* -------------------------------------------------------------------------- */
/*
 * Open and close
 */
static int sport_open(struct sport_dev *dev)
{
	int ret;

	DPRINTK("%s enter\n", __func__);

	if (!dev || !dev->regs)
		return -ENODEV;

	if (dev->in_use)
		return -EBUSY;

	memset(&dev->config, 0, sizeof(dev->config));

	dev->rx_buf = NULL;
	dev->rx_len = 0;
	dev->rx_sub_len = 0;
	dev->rx_received = 0;
	dev->count_rx_sub = 0;
	dev->rx_int_signal = 0;
	dev->rx_max = NULL;
	dev->rx_max_len = 0;	
	
	dev->tx_buf = NULL;
	dev->tx_len = 0;
	dev->tx_buf_len = 0;
	dev->tx_buf_int_len = 0;
	dev->tx_sub_len = 0;
	dev->tx_sent = 0;
	dev->count_tx_sub = 0;
	dev->tx_int_signal = 0;
	dev->tx_max = NULL;
	dev->tx_max_len = 0;

	init_completion(&dev->c);
	
	ret = request_irq(dev->err_irq, sport_err_handler, 0, SPORT_DRVNAME "-err", dev);
	if (ret) {
		pr_err("unable to request sport err irq\n");
		goto fail2;
	}

	ret = peripheral_request_list(sport_req, SPORT_DRVNAME);
	if (ret) {
		pr_err("requesting peripherals failed\n");
		goto fail3;
	}
	dev->sport_clkdiv = 0x24;

	
	tasklet_init(&dev->rx_tsk, rx_tsk_process,  (unsigned long)dev);
	tasklet_init(&dev->tx_tsk, tx_tsk_process,  (unsigned long)dev);

	dev->in_use = 1;

	return 0;

 fail3:
	free_irq(dev->err_irq, dev);
 fail2:
	free_irq(dev->rx_irq, dev);

	dev->in_use = 0;

	return ret;
}
/* -------------------------------------------------------------------------- */
static int sport_release(struct sport_dev *dev)
{
	DPRINTK("%s enter\n", __func__);

	if (!dev || !dev->regs)
		return -ENODEV;

	dev->in_use = 0;
	
	sport_stop(dev);
	
	tasklet_disable(&dev->rx_tsk);
	tasklet_kill(&dev->rx_tsk);
	tasklet_disable(&dev->tx_tsk);
	tasklet_kill(&dev->tx_tsk);		
	
	if (dev->config.dma_enabled) 
	{
		free_irq(dev->tx_irq, dev);		 
		free_irq(dev->rx_irq, dev);
	}
	free_irq(dev->err_irq, dev);

	peripheral_free_list(sport_req);
	
	dma_free_tx_buf(dev);
	dma_free_rx_buf(dev);	

	return 0;
}
/* -------------------------------------------------------------------------- */
static ssize_t sport_read(struct sport_dev *dev)
{
	struct sport_config *cfg = &dev->config;
	size_t nread=0;

	if (cfg->dma_enabled) 
	{
		if (dev->rx_received > 0)
		{
#ifdef UPDATE_MAX_SAMPLES
			update_sport_max(dev, 1);
#endif
			
			dev->read_buf_len = dev->rx_len;
			nread = dev->rx_received;			
			
			/* demux */
			spi_tdm_demux(dev->config.channels, nread/dev->config.channels, dev->rx_buf_int);
			
			dev->rx_received -= nread;
			memmove(dev->rx_buf_int, &dev->rx_buf_int[nread], dev->rx_received);
		}
	}
	
	return nread;
}
/* -------------------------------------------------------------------------- */
static ssize_t sport_write(struct sport_dev *dev)
{
	struct sport_config *cfg = &dev->config;
	size_t nwrite=0, nfree, frame_size, count;

	/* Configure dma to start transfer */
	
	if (cfg->dma_enabled) 
	{
		if (dev->tx_sent < dev->tx_buf_int_len)
		{
			count = dev->tx_sub_len;
			dev->write_buf_len = (count > dev->tx_buf_int_len) ? dev->tx_buf_int_len : count;
			nwrite = count;
			nfree = dev->tx_buf_int_len - dev->tx_sent;
			if (nwrite > nfree)
				nwrite = nfree;
			
			frame_size = 0;
			if (cfg->mode == TDM_MODE)
			{
				frame_size = cfg->channels * sport_wordsize_in_bytes(cfg->word_len);
				if ((frame_size > 0) && (nwrite % frame_size))
				{
					nwrite = (nwrite / frame_size) * frame_size;
				}
			}
			
			/* mux !!!!!! */
			nwrite = spi_tdm_mux(dev->config.channels, nwrite/dev->config.channels, &dev->tx_buf_int[dev->tx_sent]);

			dev->tx_sent += nwrite;
#ifdef UPDATE_MAX_SAMPLES
			update_sport_max(dev, 0);
#endif			
		}
	}
	
	return nwrite;
}
/*----------------------------------------------------------------------------*/
#ifdef CONFIG_PROC_FS
static struct proc_dir_entry *sport_dir_entry = NULL;

static
int sport_read_proc(char *buf, char **start, off_t offset, int cnt, int *eof, void *data)
{
	struct sport_dev *dev = &sport_device;
	int ret = 0;

	ret += sprintf(&buf[ret], "\nsport %2d:\n", dev->id);
	ret += sprintf(&buf[ret], "\tin_use=%d,\n", dev->in_use);
	ret += sprintf(&buf[ret], "\trx_len=%d,\n", dev->rx_len);
	ret += sprintf(&buf[ret], "\trx_slen=%d,\n", dev->rx_sub_len);
	ret += sprintf(&buf[ret], "\trx_rcv=%d\n", dev->rx_received);
	ret += sprintf(&buf[ret], "\trx_total=%lu\n", dev->rx_total);
	ret += sprintf(&buf[ret], "\trx_int_total=%lu\n", dev->rx_int_total);

	ret += sprintf(&buf[ret], "\ttx_len=%d,\n", dev->tx_len);
	ret += sprintf(&buf[ret], "\ttx_buf_len=%d,\n", dev->tx_buf_len);
	ret += sprintf(&buf[ret], "\ttx_buf_int_len=%d,\n", dev->tx_buf_int_len);
	ret += sprintf(&buf[ret], "\ttx_slen=%d,\n", dev->tx_sub_len);
	ret += sprintf(&buf[ret], "\ttx_sent=%d\n", dev->tx_sent);
	ret += sprintf(&buf[ret], "\ttx_int_total=%lu\n", dev->tx_int_total);
	
	ret += sprintf(&buf[ret], "\tcnt_rx=%d\n", dev->count_rx_sub);
	ret += sprintf(&buf[ret], "\tcnt_tx=%d\n", dev->count_tx_sub);	
	
	ret += sprintf(&buf[ret], "\tdelay_rx=%d\n", dev->delay_rx_sub);
	ret += sprintf(&buf[ret], "\tdelay_tx=%d\n", dev->delay_tx_sub);
	
#ifdef UPDATE_MAX_SAMPLES	
	int count, j;
	
	count = dev->rx_max_len;
	if (count > dev->tx_max_len)
		count = dev->tx_max_len;

	if (dev->rx_max && dev->tx_max)
	{
		for (j=0;j<count;j++)
		{
			ret += sprintf(&buf[ret], "\trx_max[%d] = %ld\n", j, dev->rx_max[j]);
			ret += sprintf(&buf[ret], "\ttx_max[%d] = %ld\n", j, dev->tx_max[j]);
			
			dev->tx_max[j] = 0;
			dev->rx_max[j] = 0;
		}
	}
#endif
		
	return ret;
}
#endif
/* -------------------------------------------------------------------------- */
static void rx_tsk_process(unsigned long data)
{
	struct sport_dev *dev = (struct sport_dev *)data;
	if (dev && dev->in_use)
		sport_read(dev);
}
/* -------------------------------------------------------------------------- */
static void tx_tsk_process(unsigned long data)
{
	struct sport_dev *dev = (struct sport_dev *)data;
	if (dev && dev->in_use)
		sport_write(dev);
}
/* -------------------------------------------------------------------------- */
void sport_deinit(void* context)
{
	struct sport_dev *dev = &sport_device;
	sport_release(dev);
#ifdef CONFIG_PROC_FS
	if (sport_dir_entry)
			remove_proc_entry(SPORT_DRVNAME, NULL);
#endif

	sysclk_source_remove(dev->sysclk_id);
}
/* -------------------------------------------------------------------------- */
int sport_init(void* context)
{
	int result;
	struct sport_dev *dev = &sport_device;
	struct sport_stream_config_profile *stream_config = &sport_stream_config_list[PROFILE_ID];
	struct sport_config *config = &sport_config_list[PROFILE_ID];
	
	dev->sysclk_id = sysclk_source_add(&dev->sysclk_config);
	if (dev->sysclk_id < 0)
		pr_err("sysclk_source_add: drvname='%s', error=%d\n", SPORT_DRVNAME, dev->sysclk_id);
	
	result = sport_open(dev);
	if (!result)
	{
		result = sport_stream_configure(dev, &(stream_config->config));
		if (!result)
		{
			result = sport_configure(dev, config);
			if (!result)
			{
				/**/
			}
			else
			{
				sport_release(dev);
			}
		}
		else
		{
			sport_release(dev);
		}
	}
	
#ifdef CONFIG_PROC_FS
	sport_dir_entry = create_proc_entry(SPORT_DRVNAME, 0444, NULL);

	if (sport_dir_entry)
			sport_dir_entry->read_proc = &sport_read_proc;
#endif  

	return result;
}
/* -------------------------------------------------------------------------- */
