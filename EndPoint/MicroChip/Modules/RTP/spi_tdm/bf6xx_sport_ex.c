/*
 * simple char interface to Blackfin SPORT peripheral
 *
 * 
 * Pwnd by q2l
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

#include "bf6xx_sport_ex.h"
#include "spi_tdm.h"
#include "clk-common.h"


#define UPDATE_MAX_SAMPLES
#define SPORT_DRVNAME		"bfin_sport_ex"


#ifdef DEBUG
#define BFPRINTK(fmt, args...) printk("%s: " fmt, __FUNCTION__ , ## args)
#else
#define BFPRINTK(fmt, args...)
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

struct s_sport_dev {
	int	id;
	int in_use;
	int dma_rx_chan;
	int dma_tx_chan;
	unsigned short pins_req[16];

	int rx_err_irq;
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
	unsigned long rx_overflow;

	int tx_err_irq;
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
	unsigned long tx_underflow;


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

	int sport_clkdiv;

	struct completion c;

	volatile struct sport_register *tx_regs;
	volatile struct sport_register *rx_regs;

	struct sport_config config;
	
	struct tasklet_struct rx_tsk;
	struct tasklet_struct tx_tsk;
	
	struct s_sysclk_config	sysclk_config;
	int						sysclk_id;
};

#define SPORT_PARAMS(x) \
		.id          = x, \
		.in_use      = 0, \
		.dma_rx_chan	= CH_SPORT##x##_RX, \
		.dma_tx_chan	= CH_SPORT##x##_TX, \
		.tx_err_irq		= IRQ_SPORT##x##_TX_STAT, \
		.rx_err_irq		= IRQ_SPORT##x##_RX_STAT, \
		.tx_regs		= (void *)SPORT##x##_CTL_A, \
		.rx_regs		= (void *)SPORT##x##_CTL_B, \
		.pins_req		= { \
        P_SPORT##x##_ACLK, P_SPORT##x##_AFS, P_SPORT##x##_AD0, P_SPORT##x##_AD1, P_SPORT##x##_ATDV,     \
        P_SPORT##x##_BCLK, P_SPORT##x##_BFS, P_SPORT##x##_BD0, P_SPORT##x##_BD1, P_SPORT##x##_BTDV, 0 }, \


static struct s_sport_dev sport_device = {
	SPORT_PARAMS(1)
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

static inline int sport_start(struct s_sport_dev *sport_dev);
static inline int sport_stop(struct s_sport_dev *sport_dev);

static inline int sport_rx_dma_init(struct s_sport_dev *sport_dev);
static inline int sport_tx_dma_init(struct s_sport_dev *sport_dev);

static ssize_t sport_read(struct s_sport_dev *sport_dev);
static ssize_t sport_write(struct s_sport_dev *sport_dev);
static int sport_release(struct s_sport_dev *sport_dev);
static int sport_open(struct s_sport_dev *sport_dev);
/* -------------------------------------------------------------------------- */
#ifdef UPDATE_MAX_SAMPLES
static int update_sport_max(struct s_sport_dev *sport_dev, int rxtx)
{
	short 		*pbasebuf = NULL;
	short 		*pbuf = NULL;
	size_t 		count = 0;
	long 		*pbasemaxbuf = NULL;
	long 		*pmaxbuf = NULL;
	size_t 		max_count = 0;
	int 		i, j;
	
	if (sport_dev)
	{
		if (rxtx)
		{
			pbasebuf = (short *)sport_dev->rx_buf_int;
			count = sport_dev->rx_received / 2;
			pbasemaxbuf = sport_dev->rx_max;
			max_count = sport_dev->rx_max_len;
		}
		else
		{
			pbasebuf = (short *)sport_dev->tx_buf_int;
			count = sport_dev->tx_sent / 2;
			pbasemaxbuf = sport_dev->tx_max;
			max_count = sport_dev->tx_max_len;
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
static int dma_free_tx_buf(struct s_sport_dev *sport_dev)
{
	if (sport_dev)
	{
		if (sport_dev->tx_buf)
		{
			dma_free_coherent(NULL, sport_dev->tx_buf_len, sport_dev->tx_buf, 0);
			sport_dev->tx_buf = NULL;
		}
		
		if (sport_dev->tx_buf_int)
		{
			dma_free_coherent(NULL, sport_dev->tx_buf_int_len, sport_dev->tx_buf_int, 0);
			sport_dev->tx_buf_int = NULL;
		}		
		
		if (sport_dev->dma_tx_desc)
		{
			dma_free_coherent(NULL, sport_dev->count_tx_sub*sizeof(struct dmasg), sport_dev->dma_tx_desc, 0);
			sport_dev->count_tx_sub = 0;
			sport_dev->dma_tx_desc = NULL;
		}
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int dma_free_rx_buf(struct s_sport_dev *sport_dev)
{
	if (sport_dev)
	{
		if (sport_dev->rx_buf)
		{
			dma_free_coherent(NULL, sport_dev->rx_len, sport_dev->rx_buf, 0);
			sport_dev->rx_buf = NULL;
		}
		
		if (sport_dev->rx_buf_int)
		{
			dma_free_coherent(NULL, sport_dev->rx_len, sport_dev->rx_buf_int, 0);
			sport_dev->rx_buf_int = NULL;
		}		
		
		if (sport_dev->dma_rx_desc)
		{
			dma_free_coherent(NULL, sport_dev->count_rx_sub*sizeof(struct dmasg), sport_dev->dma_rx_desc, 0);			
			sport_dev->count_rx_sub = 0;
			sport_dev->dma_rx_desc = NULL;
		}		
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
static int sport_stream_configure(struct s_sport_dev *sport_dev, struct sport_stream_config *stream_config)
{
	dma_addr_t addr;

	pr_debug("sport stream configure: rx_len=0x%08X, tx_len=0x%08X\n", stream_config->rx_len, stream_config->tx_len);
	if ((sport_dev->rx_buf == NULL) || ((sport_dev->rx_buf != NULL) && (sport_dev->rx_len != stream_config->rx_len)))	
	{
		dma_free_rx_buf(sport_dev);
		
		if (stream_config->rx_len > 0)
		{	
			sport_dev->rx_len = stream_config->rx_len;
			sport_dev->read_buf_len = (stream_config->read_buf_len > sport_dev->rx_len) ? sport_dev->rx_len : stream_config->read_buf_len;
			sport_dev->count_rx_sub = (stream_config->count_rx_sub <= 0) ? AUTOBUFFER_COUNT : stream_config->count_rx_sub;
			sport_dev->delay_rx_sub = (stream_config->delay_rx_sub <= 0) ? AUTOBUFFER_DELAY : stream_config->delay_rx_sub;
			
			sport_dev->rx_sub_len = sport_dev->rx_len/sport_dev->count_rx_sub;
			sport_dev->rx_buf = dma_alloc_coherent(NULL, sport_dev->rx_len, &addr, 0);		
			sport_dev->rx_buf_int = dma_alloc_coherent(NULL, sport_dev->rx_len, &addr, 0);
			sport_dev->sysclk_config.tick_inc = sport_dev->rx_sub_len;			

			sport_dev->dma_rx_desc =  dma_alloc_coherent(NULL, sport_dev->count_rx_sub*sizeof(struct dmasg), &addr, 0);
			sport_dev->curr_rx_desc = sport_dev->dma_rx_desc;
		}
	}
	
	if ((sport_dev->tx_buf == NULL) || ((sport_dev->tx_buf != NULL) && (sport_dev->tx_len != stream_config->tx_len)))	
	{
		dma_free_tx_buf(sport_dev);
				
		if (stream_config->tx_len > 0)
		{	
			sport_dev->tx_len = stream_config->tx_len;
			sport_dev->tx_buf_len = sport_dev->tx_len;
			sport_dev->tx_buf_int_len = (sport_dev->tx_len<<1);

			sport_dev->write_buf_len = (stream_config->write_buf_len > sport_dev->tx_len) ? sport_dev->tx_len : stream_config->write_buf_len;
			sport_dev->count_tx_sub = (stream_config->count_tx_sub <= 0) ? AUTOBUFFER_COUNT : stream_config->count_tx_sub;
			sport_dev->delay_tx_sub = (stream_config->delay_tx_sub <= 0) ? AUTOBUFFER_DELAY : stream_config->delay_tx_sub;		
			sport_dev->tx_sub_len = sport_dev->tx_len/sport_dev->count_tx_sub;			
			
			sport_dev->tx_buf = dma_alloc_coherent(NULL, sport_dev->tx_buf_len, &addr, 0);
			sport_dev->tx_buf_int = dma_alloc_coherent(NULL, sport_dev->tx_buf_int_len, &addr, 0);			
			memset(sport_dev->tx_buf, 0, sport_dev->tx_buf_len);
			memset(sport_dev->tx_buf_int, 0, sport_dev->tx_buf_int_len);
			
			sport_dev->dma_tx_desc =  dma_alloc_coherent(NULL, sport_dev->count_tx_sub*sizeof(struct dmasg), &addr, 0);			
			sport_dev->curr_tx_desc = sport_dev->dma_tx_desc;
		}
	}	
	
	return 0;	
}
/* -------------------------------------------------------------------------- */
static int maybe_request_irq(int irq, irq_handler_t handler,
	unsigned long flags, const char *name, struct s_sport_dev *sport_dev)
{
	int ret = 0;

	if (irq == -1)
		return ret;

	ret = request_irq(irq, handler, flags, name, sport_dev);
	if (ret)
		pr_err("unable to request irq %s\n", name);

	return ret;
}
/* -------------------------------------------------------------------------- */
static void maybe_free_irq(int irq, struct s_sport_dev *sport_dev)
{
	if (irq != -1)
		free_irq(irq, sport_dev);
}
/* -------------------------------------------------------------------------- */
static int sport_configure(struct s_sport_dev *sport_dev, struct sport_config *config)
{
	volatile struct dma_register *rx_regs, *tx_regs;
	unsigned int rx_spctl, tx_spctl;
	unsigned int rx_spmctl, tx_spmctl;
	unsigned int rx_spcs0, tx_spcs0;
	unsigned int rx_div, tx_div;
	unsigned int data_format, frame_delay, channels;
	u_long sclk;
	unsigned int clkdiv, fsdiv;
	struct sport_config *old_cfg = &sport_dev->config;
	
	rx_spctl = tx_spctl = 0;
	rx_spmctl = tx_spmctl = 0;
	rx_spcs0 = tx_spcs0 = 0;
	rx_div = tx_div = 0;
	sclk = clkdiv = fsdiv = 0;
	data_format = 0;
	frame_delay = 0;
	channels = 0;

	memcpy(old_cfg, config, sizeof(*config));

	if ((sport_dev->tx_regs->spctl & SPORT_CTL_SPENPRI) ||
		(sport_dev->rx_regs->spctl & SPORT_CTL_SPENPRI))
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
		if ((sport_dev->rx_max_len != config->channels) || !sport_dev->rx_max)
		{
			if (sport_dev->rx_max)
			{
				kfree(sport_dev->rx_max);
				sport_dev->rx_max = NULL;
				sport_dev->rx_max_len = 0;
			}
			sport_dev->rx_max_len = config->channels;
			sport_dev->rx_max = kmalloc(sport_dev->rx_max_len * sizeof(long), GFP_KERNEL);
			if (sport_dev->rx_max)
			{
				memset(sport_dev->rx_max, 0, sport_dev->rx_max_len * sizeof(long));
			}
		}
		
		/*  init tx max buff */
		if ((sport_dev->tx_max_len != config->channels) || !sport_dev->tx_max)
		{
			if (sport_dev->tx_max)
			{
				kfree(sport_dev->tx_max);
				sport_dev->tx_max = NULL;
				sport_dev->tx_max_len = 0;
			}
			sport_dev->tx_max_len = config->channels;
			sport_dev->tx_max = kmalloc(sport_dev->tx_max_len * sizeof(long), GFP_KERNEL);
			if (sport_dev->tx_max)
			{
				memset(sport_dev->tx_max, 0, sport_dev->tx_max_len * sizeof(long));
			}
		}
#endif

		frame_delay = (config->frame_delay << 4) & SPORT_MCTL_MFD;
		channels = ((config->channels-1) << 8) & SPORT_MCTL_WSIZE;
		rx_spmctl = SPORT_MCTL_MCE | SPORT_MCTL_MCPDE
				| frame_delay
				| channels;
		tx_spmctl = SPORT_MCTL_MCE | SPORT_MCTL_MCPDE
				| frame_delay
				| channels;

		rx_spcs0 = (1<<config->channels)-1;
		tx_spcs0 = (1<<config->channels)-1;
				
	} else if (config->mode == I2S_MODE) {

		rx_spctl |= SPORT_CTL_OPMODE | SPORT_CTL_CKRE | SPORT_CTL_LFS;
		tx_spctl |= SPORT_CTL_OPMODE | SPORT_CTL_CKRE | SPORT_CTL_LFS;
	} else {

		if (config->lsb_first)
		{
			rx_spctl |= SPORT_CTL_LSBF;
			tx_spctl |= SPORT_CTL_LSBF;
		}
		if (config->act_low)
		{
			rx_spctl |= SPORT_CTL_LFS;
			tx_spctl |= SPORT_CTL_LFS;
		}
		if (config->tckfe)
		{
			rx_spctl |= SPORT_CTL_CKRE;
			tx_spctl |= SPORT_CTL_CKRE;
		}
		if (config->fsync)
		{
			rx_spctl |= SPORT_CTL_FSR;
			tx_spctl |= SPORT_CTL_FSR;
		}
		if (config->data_indep)
		{
			rx_spctl |= SPORT_CTL_DIFS;
			tx_spctl |= SPORT_CTL_DIFS;
		}
		if (config->late_fsync)
		{
			rx_spctl |= SPORT_CTL_LAFS;
			tx_spctl |= SPORT_CTL_LAFS;
		}
	}

	/* Using internal clock */
	if (config->int_clk) {
		sclk = get_sclk();

		if (config->serial_clk <= 0 || config->serial_clk > sclk / 2)
		{
			pr_err("serial_clk value fail\n");
			return -EINVAL;
		}
		clkdiv = (sclk / config->serial_clk) - 1;
		if (config->fsync_clk)
			fsdiv = (config->serial_clk / config->fsync_clk) - 1;

		rx_spctl |= (SPORT_CTL_ICLK | SPORT_CTL_IFS);
		tx_spctl |= (SPORT_CTL_ICLK | SPORT_CTL_IFS);
	}
	rx_div = (clkdiv & SPORT_DIV_CLKDIV) | ((fsdiv & SPORT_DIV_FSDIV) << 16);
	tx_div = (clkdiv & SPORT_DIV_CLKDIV) | ((fsdiv & SPORT_DIV_FSDIV) << 16);
	
	/* Setting data format */
	/*
	 * Data Format of BF60X different from Data Format of BF52X
	 * */
	data_format = config->data_format;
	if (config->data_format == ALAW_FORMAT)
		data_format = 0x3;
	if (config->data_format == ULAW_FORMAT)
		data_format = 0x2;
		
	rx_spctl |= (data_format << 1) & SPORT_CTL_DTYPE;	/* Bits DTYPE */
	tx_spctl |= (data_format << 1) & SPORT_CTL_DTYPE;	/* Bits DTYPE */

	if (config->word_len >= 5 && config->word_len <= 32) {
		rx_spctl |= ((config->word_len - 1) << 4) & SPORT_CTL_SLEN;	/* Bits SLEN */
		tx_spctl |= ((config->word_len - 1) << 4) & SPORT_CTL_SLEN;	/* Bits SLEN */
	} else
	{
		pr_err("word lenght value fail\n");
		return -EINVAL;
	}


	/* Setting TX/RX mode */
	rx_spctl &= ~SPORT_CTL_SPTRAN;
	tx_spctl |= SPORT_CTL_SPTRAN;
	
	/* Setting RX/RX regs */	
	bfin_write(&sport_dev->rx_regs->spctl,	rx_spctl);
	bfin_write(&sport_dev->rx_regs->div,	rx_div);
	bfin_write(&sport_dev->rx_regs->spmctl,	rx_spmctl);
	bfin_write(&sport_dev->rx_regs->spcs0,	rx_spcs0);
	bfin_write(&sport_dev->tx_regs->spctl,	tx_spctl);
	bfin_write(&sport_dev->tx_regs->div,	tx_div);
	bfin_write(&sport_dev->tx_regs->spmctl,	tx_spmctl);
	bfin_write(&sport_dev->tx_regs->spcs0,	tx_spcs0);

	SSYNC();
	
	/* enabled DMAs */
	if (config->dma_enabled)
	{
		sport_rx_dma_init(sport_dev);
		sport_tx_dma_init(sport_dev);
		sport_start(sport_dev);
	}

	pr_debug("rx regs:\n");
	pr_debug("spctl:0x%08x, div:0x%08x, spmctl:0x%08x, spcs0:0x%08x\n",
				sport_dev->rx_regs->spctl, sport_dev->rx_regs->div, sport_dev->rx_regs->spmctl, sport_dev->rx_regs->spcs0);

	pr_debug("tx regs:\n");
	pr_debug("spctl:0x%08x, div:0x%08x, spmctl:0x%08x, spcs0:0x%08x\n",
				sport_dev->tx_regs->spctl, sport_dev->tx_regs->div, sport_dev->tx_regs->spmctl, sport_dev->tx_regs->spcs0);

	rx_regs = dma_ch[sport_dev->dma_rx_chan].regs;
	pr_debug("dma rx:\n");
	pr_debug("start_addr : 0x%08lX,   buf addr : 0x%p\n "
			 "cfg : 0x%08lX,  irq_status : 0x%08lX\n"	
			 "x_count : 0x%lX, x_modify : 0x%lX, y_count : 0x%lX, y_modify : 0x%lX \n"
			 ,
				rx_regs->start_addr, sport_dev->rx_buf,
				rx_regs->cfg, rx_regs->irq_status,
				rx_regs->x_count, rx_regs->x_modify,
				rx_regs->y_count, rx_regs->y_modify);
				
	tx_regs = dma_ch[sport_dev->dma_tx_chan].regs;
	pr_debug("dma tx:\n");
	pr_debug("start_addr : 0x%08lX,   buf addr : 0x%p\n "
			 "cfg : 0x%08lX,  irq_status : 0x%08lX\n"
			 "x_count : 0x%lX, x_modify : 0x%lX, y_count : 0x%lX, y_modify : 0x%lX \n"
			 ,
				tx_regs->start_addr, sport_dev->tx_buf,
				tx_regs->cfg, tx_regs->irq_status,
				tx_regs->x_count, tx_regs->x_modify,
				tx_regs->y_count, tx_regs->y_modify);

	return 0;
}
/* -------------------------------------------------------------------------- */
static inline uint16_t sport_wordsize(int word_len)
{
	uint16_t wordsize = 0;

	if (word_len <= 8)
		wordsize = WDSIZE_8 | PSIZE_8;
	else if (word_len <= 16)
		wordsize = WDSIZE_16 | PSIZE_16;
	else if (word_len <= 32)
		wordsize = WDSIZE_32 | PSIZE_32;
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
static inline int sport_start(struct s_sport_dev *sport_dev)
{
	if (sport_dev->config.dma_enabled)
	{
		enable_dma(sport_dev->dma_rx_chan);
		enable_dma(sport_dev->dma_tx_chan);
	}
	bfin_write_or(&sport_dev->tx_regs->spctl, SPORT_CTL_SPENPRI);
	bfin_write_or(&sport_dev->rx_regs->spctl, SPORT_CTL_SPENPRI);
	SSYNC();

	return 0;
}
/* -------------------------------------------------------------------------- */
static inline int sport_stop(struct s_sport_dev *sport_dev)
{
	bfin_write_and(&sport_dev->tx_regs->spctl, ~SPORT_CTL_SPENPRI);
	bfin_write_and(&sport_dev->rx_regs->spctl, ~SPORT_CTL_SPENPRI);
	SSYNC();
	if (sport_dev->config.dma_enabled)
	{
		disable_dma(sport_dev->dma_rx_chan);
		disable_dma(sport_dev->dma_tx_chan);
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
static inline int sport_rx_dma_init(struct s_sport_dev *sport_dev)
{
	unsigned int x_count;
	unsigned int y_count;
	unsigned int dma_cfg;
	unsigned int fragsize, fragcount;

	struct sport_config *cfg = &sport_dev->config;
	int word_bytes = (cfg->word_len + 7) / 8;
	
	
	if (word_bytes == 3)
		word_bytes = 4;

	fragcount	= sport_dev->count_rx_sub;
	fragsize	= sport_dev->rx_len/fragcount;

 	x_count 	= fragsize / word_bytes;
 	y_count	= 0;
	
	dma_cfg 	= (WNR | sport_wordsize(cfg->word_len) | DMAFLOW_LIST | NDSIZE_6 | DI_EN );
	
	if (y_count != 0)
		dma_cfg |= DMA2D;

	setup_desc(sport_dev->dma_rx_desc, sport_dev->rx_buf, fragcount, fragsize,
			dma_cfg | DMAEN, x_count, y_count, word_bytes);
			
	sport_dev->curr_rx_desc = sport_dev->dma_rx_desc;
	set_dma_next_desc_addr(sport_dev->dma_rx_chan, sport_dev->curr_rx_desc);
	set_dma_x_count(sport_dev->dma_rx_chan, 0);
	set_dma_x_modify(sport_dev->dma_rx_chan, word_bytes);
	set_dma_y_count(sport_dev->dma_rx_chan, 0);
	set_dma_y_modify(sport_dev->dma_rx_chan, word_bytes);
	set_dma_config(sport_dev->dma_rx_chan, dma_cfg);
	set_dma_curr_addr(sport_dev->dma_rx_chan, sport_dev->curr_rx_desc->start_addr);
	
	SSYNC();				

	return 0;
}
/* -------------------------------------------------------------------------- */
static inline int sport_tx_dma_init(struct s_sport_dev *sport_dev)
{
	unsigned int x_count;
	unsigned int y_count;
	unsigned int dma_cfg;
	unsigned int fragsize, fragcount;

	struct sport_config *cfg = &sport_dev->config;
	int word_bytes = (cfg->word_len + 7) / 8;
	
	
	if (word_bytes == 3)
		word_bytes = 4;

	fragcount	= sport_dev->count_tx_sub;
	fragsize	= sport_dev->tx_len/fragcount;

 	x_count 	= fragsize / word_bytes;
 	y_count	= 0;
	
	dma_cfg 	= (sport_wordsize(cfg->word_len) | DMAFLOW_LIST | NDSIZE_6 | DI_EN );
	
	if (y_count != 0)
		dma_cfg |= DMA2D;

	setup_desc(sport_dev->dma_tx_desc, sport_dev->tx_buf, fragcount, fragsize,
			dma_cfg | DMAEN, x_count, y_count, word_bytes);
			
	sport_dev->curr_tx_desc = sport_dev->dma_tx_desc;
	set_dma_next_desc_addr(sport_dev->dma_tx_chan, sport_dev->curr_tx_desc);
	set_dma_config(sport_dev->dma_tx_chan, dma_cfg);
	
	SSYNC();				

	return 0;
}
/* -------------------------------------------------------------------------- */
static inline int get_dma_curr_rx_desc_index(struct s_sport_dev *sport_dev)
{
	unsigned long desc, curr_desc;
	int index = -1;
	
	desc = (unsigned long)sport_dev->dma_rx_desc;
	curr_desc = (unsigned long)get_dma_curr_desc_ptr(sport_dev->dma_rx_chan);

	index = (curr_desc - desc)/sizeof(struct dmasg);
	if (index >= sport_dev->count_rx_sub) index -= sport_dev->count_rx_sub;
	return index;
}
/* -------------------------------------------------------------------------- */
static inline int get_dma_curr_tx_desc_index(struct s_sport_dev *sport_dev)
{
	unsigned long desc, curr_desc;
	int index = -1;
	
	desc = (unsigned long)sport_dev->dma_tx_desc;
	curr_desc = (unsigned long)get_dma_curr_desc_ptr(sport_dev->dma_tx_chan);

	index = (curr_desc - desc)/sizeof(struct dmasg);
	if (index >= sport_dev->count_tx_sub) index -= sport_dev->count_tx_sub;
	return index;
}
/* -------------------------------------------------------------------------- */
static inline unsigned char *get_curr_rx_buf(struct s_sport_dev *sport_dev)
{
	unsigned char *rx_buf=NULL;
	int index;
	index = get_dma_curr_rx_desc_index(sport_dev);
	
	index = (index - sport_dev->delay_rx_sub + sport_dev->count_rx_sub);
	if (index >= sport_dev->count_rx_sub) index -= sport_dev->count_rx_sub;
	
	rx_buf = (unsigned char *)sport_dev->dma_rx_desc[index].start_addr;	
	
	return rx_buf;
}
/* -------------------------------------------------------------------------- */
static inline unsigned char *get_curr_tx_buf(struct s_sport_dev *sport_dev)
{
	unsigned char *tx_buf=NULL;
	int index;
	index = get_dma_curr_tx_desc_index(sport_dev);
	
	index = (index - sport_dev->delay_tx_sub + sport_dev->count_tx_sub);
	if (index >= sport_dev->count_tx_sub) index -= sport_dev->count_tx_sub;
	
	tx_buf = (unsigned char *)sport_dev->dma_tx_desc[index].start_addr;	
	
	return tx_buf;
}
/* -------------------------------------------------------------------------- */
static irqreturn_t sport_rx_handler(int irq, void *dev_id)
{
	unsigned char *rx_buf;
	struct s_sport_dev *sport_dev = dev_id;	
	int nmove;

	clear_dma_irqstat(sport_dev->dma_rx_chan);
	SSYNC();

	rx_buf = get_curr_rx_buf(sport_dev);
	if (rx_buf)
	{
		if ((sport_dev->rx_received + sport_dev->rx_sub_len) > sport_dev->rx_len)
		{
			nmove = (sport_dev->rx_received + sport_dev->rx_sub_len) - sport_dev->rx_len;
			sport_dev->rx_received -= nmove;
			memmove(sport_dev->rx_buf_int, &sport_dev->rx_buf_int[nmove], sport_dev->rx_received);
		}
		memcpy(&sport_dev->rx_buf_int[sport_dev->rx_received], rx_buf, sport_dev->rx_sub_len);
		sport_dev->rx_received += sport_dev->rx_sub_len;
	}

	if (sport_dev->rx_received >= sport_dev->read_buf_len)
	{
		if (sport_dev->rx_int_signal == 0)
		{
			sport_dev->rx_int_signal = 1;
		}
	}
	tasklet_schedule(&sport_dev->rx_tsk);
	sport_dev->rx_total += sport_dev->rx_sub_len;
	sport_dev->rx_int_total++;
	if (sport_dev->sysclk_id >= 0)
		sysclk_source_tick(sport_dev->sysclk_id, sport_dev->rx_total);		
		
	
	/* tx error process in rx handler */
	if (sport_dev->tx_regs->spctl & SPORT_CTL_DERRPRI)
	{
		sport_dev->tx_underflow++;
	}

	return IRQ_HANDLED;
}
/* -------------------------------------------------------------------------- */
static irqreturn_t sport_tx_handler(int irq, void *dev_id)
{
	unsigned char *tx_buf;			
	struct s_sport_dev *sport_dev = dev_id;
	
	clear_dma_irqstat(sport_dev->dma_tx_chan);
	SSYNC();
	
	tx_buf = get_curr_tx_buf(sport_dev);
	if (tx_buf)
	{
		if (sport_dev->tx_sent >= sport_dev->tx_sub_len)
		{
			memcpy(tx_buf, sport_dev->tx_buf_int, sport_dev->tx_sub_len);
			sport_dev->tx_sent -= sport_dev->tx_sub_len;
			memmove(sport_dev->tx_buf_int, &sport_dev->tx_buf_int[sport_dev->tx_sub_len], sport_dev->tx_sent);
		}
		else
		{
			memset(tx_buf, 0, sport_dev->tx_sub_len);	
			memcpy(tx_buf, sport_dev->tx_buf_int, sport_dev->tx_sent);
			sport_dev->tx_sent = 0;
		}
	}
	
	tasklet_schedule(&sport_dev->tx_tsk);
	
	sport_dev->tx_int_total++;

	/* rx error process in tx handler */
	if (sport_dev->rx_regs->spctl & SPORT_CTL_DERRPRI)
	{
		sport_dev->rx_overflow++;
	}

	return IRQ_HANDLED;
}
/* -------------------------------------------------------------------------- */
static irqreturn_t sport_err_handler(int irq, void *dev_id)
{
	struct s_sport_dev *sport_dev = dev_id;

	if (sport_dev->tx_regs->spctl & SPORT_CTL_DERRPRI)
	{
		sport_dev->tx_underflow++;
		BFPRINTK("sport error: tx_underflow=%ld\n", sport_dev->tx_underflow);
	}
	if (sport_dev->rx_regs->spctl & SPORT_CTL_DERRPRI)
	{
		sport_dev->rx_overflow++;
		BFPRINTK("sport error: rx_overflow=%ld\n", sport_dev->rx_overflow);
	}

	return IRQ_HANDLED;
}
/* -------------------------------------------------------------------------- */
static int sport_request_resource(struct s_sport_dev *sport_dev)
{
	int ret=0;
	
#if defined(CONFIG_PINCTRL)
	ret = pinmux_request_list(sport_dev->pins_req, SPORT_DRVNAME);
#else
	/* need declare pdev */
	ret = peripheral_request_list(sport_dev->pins_req, SPORT_DRVNAME);
#endif
	if (ret) {
		pr_err("Unable to request sport pins, error=%d\n", ret);
		return ret;
	}

	ret = request_dma(sport_dev->dma_tx_chan, SPORT_DRVNAME " DMA TX Data");
	if (ret) {
		pr_err("Unable to allocate DMA channel for sport tx\n");
		goto err_tx_dma;
	}
	set_dma_callback(sport_dev->dma_tx_chan, sport_tx_handler, sport_dev);

	ret = request_dma(sport_dev->dma_rx_chan, SPORT_DRVNAME " DMA RX Data");
	if (ret) {
		pr_err("Unable to allocate DMA channel for sport rx\n");
		goto err_rx_dma;
	}
	set_dma_callback(sport_dev->dma_rx_chan, sport_rx_handler, sport_dev);

	ret = maybe_request_irq(sport_dev->tx_err_irq, sport_err_handler,
			0, SPORT_DRVNAME " TX ERROR", sport_dev);
	if (ret) {
		pr_err("Unable to allocate TX STATE IRQ for sport\n");
		goto err_tx_irq;
	}
	
	ret = maybe_request_irq(sport_dev->rx_err_irq, sport_err_handler,
			0, SPORT_DRVNAME " RX ERROR", sport_dev);
	if (ret) {
		pr_err("Unable to allocate RX STATE IRQ for sport\n");
		goto err_rx_irq;
	}

	return 0;
err_rx_irq:
	maybe_free_irq(sport_dev->tx_err_irq, sport_dev);
err_tx_irq:
	free_dma(sport_dev->dma_rx_chan);
err_rx_dma:
	free_dma(sport_dev->dma_tx_chan);
err_tx_dma:

#if defined(CONFIG_PINCTRL)
	pinmux_free_list(sport_dev->pins_req);
#else
	peripheral_free_list(sport_dev->pins_req);
#endif

	return ret;
}
/* -------------------------------------------------------------------------- */
static void sport_free_resource(struct s_sport_dev *sport_dev)
{
	maybe_free_irq(sport_dev->rx_err_irq, sport_dev);
	maybe_free_irq(sport_dev->tx_err_irq, sport_dev);
	free_dma(sport_dev->dma_rx_chan);
	free_dma(sport_dev->dma_tx_chan);
	
#if defined(CONFIG_PINCTRL)
	pinmux_free_list(sport_dev->pins_req);
#else
	peripheral_free_list(sport_dev->pins_req);
#endif
}
/* -------------------------------------------------------------------------- */
/*
 * Open and close
 */
static int sport_open(struct s_sport_dev *sport_dev)
{
	int ret;

	BFPRINTK("%s enter\n", __func__);

	if (!sport_dev || !sport_dev->tx_regs || !sport_dev->rx_regs)
		return -ENODEV;

	if (sport_dev->in_use)
		return -EBUSY;

	memset(&sport_dev->config, 0, sizeof(sport_dev->config));

	sport_dev->rx_buf = NULL;
	sport_dev->rx_len = 0;
	sport_dev->rx_sub_len = 0;
	sport_dev->rx_received = 0;
	sport_dev->count_rx_sub = 0;
	sport_dev->rx_int_signal = 0;
	sport_dev->rx_max = NULL;
	sport_dev->rx_max_len = 0;
	sport_dev->rx_int_total = 0;
	sport_dev->rx_overflow = 0;
	
	sport_dev->tx_buf = NULL;
	sport_dev->tx_len = 0;
	sport_dev->tx_buf_len = 0;
	sport_dev->tx_buf_int_len = 0;
	sport_dev->tx_sub_len = 0;
	sport_dev->tx_sent = 0;
	sport_dev->count_tx_sub = 0;
	sport_dev->tx_int_signal = 0;
	sport_dev->tx_max = NULL;
	sport_dev->tx_max_len = 0;
	sport_dev->tx_int_total = 0;
	sport_dev->tx_underflow = 0;

	init_completion(&sport_dev->c);

	ret = sport_request_resource(sport_dev);
	if (!ret)
	{
		sport_dev->sport_clkdiv = 0x24;

		tasklet_init(&sport_dev->rx_tsk, rx_tsk_process,  (unsigned long)sport_dev);
		tasklet_init(&sport_dev->tx_tsk, tx_tsk_process,  (unsigned long)sport_dev);

		sport_dev->in_use = 1;
	}

	return 0;
}
/* -------------------------------------------------------------------------- */
static int sport_release(struct s_sport_dev *sport_dev)
{
	BFPRINTK("%s enter\n", __func__);

	if (!sport_dev  || !sport_dev->tx_regs || !sport_dev->rx_regs)
		return -ENODEV;

	sport_dev->in_use = 0;
	
	sport_stop(sport_dev);
	
	tasklet_disable(&sport_dev->rx_tsk);
	tasklet_kill(&sport_dev->rx_tsk);
	tasklet_disable(&sport_dev->tx_tsk);
	tasklet_kill(&sport_dev->tx_tsk);		
	
	sport_free_resource(sport_dev);
	
	dma_free_tx_buf(sport_dev);
	dma_free_rx_buf(sport_dev);	

	return 0;
}
/* -------------------------------------------------------------------------- */
static ssize_t sport_read(struct s_sport_dev *sport_dev)
{
	struct sport_config *cfg = &sport_dev->config;
	size_t nread=0;

	if (cfg->dma_enabled) 
	{
		if (sport_dev->rx_received > 0)
		{
#ifdef UPDATE_MAX_SAMPLES
			update_sport_max(sport_dev, 1);
#endif
			
			sport_dev->read_buf_len = sport_dev->rx_len;
			nread = sport_dev->rx_received;			
			
			/* demux */
			spi_tdm_demux(sport_dev->config.channels, nread/sport_dev->config.channels, sport_dev->rx_buf_int);
			
			sport_dev->rx_received -= nread;
			memmove(sport_dev->rx_buf_int, &sport_dev->rx_buf_int[nread], sport_dev->rx_received);
		}
	}
	
	return nread;
}
/* -------------------------------------------------------------------------- */
static ssize_t sport_write(struct s_sport_dev *sport_dev)
{
	struct sport_config *cfg = &sport_dev->config;
	size_t nwrite=0, nfree, frame_size, count;

	/* Configure dma to start transfer */
	
	if (cfg->dma_enabled) 
	{
		if (sport_dev->tx_sent < sport_dev->tx_buf_int_len)
		{
			count = sport_dev->tx_sub_len;
			sport_dev->write_buf_len = (count > sport_dev->tx_buf_int_len) ? sport_dev->tx_buf_int_len : count;
			nwrite = count;
			nfree = sport_dev->tx_buf_int_len - sport_dev->tx_sent;
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
			nwrite = spi_tdm_mux(sport_dev->config.channels, nwrite/sport_dev->config.channels, &sport_dev->tx_buf_int[sport_dev->tx_sent]);

			sport_dev->tx_sent += nwrite;
#ifdef UPDATE_MAX_SAMPLES
			update_sport_max(sport_dev, 0);
#endif			
		}
	}
	
	return nwrite;
}
/*----------------------------------------------------------------------------*/
#ifdef CONFIG_PROC_FS

static int bfin_sport_ex_proc_show(struct seq_file *m, void *v)
{
	struct s_sport_dev *sport_dev = &sport_device;
	int ret = 0;
#ifdef UPDATE_MAX_SAMPLES	
	int count, j;
#endif

	ret += seq_printf(m, "\nsport %2d:\n", sport_dev->id);
	ret += seq_printf(m, "\tin_use=%d,\n", sport_dev->in_use);
	ret += seq_printf(m, "\trx_len=%d,\n", sport_dev->rx_len);
	ret += seq_printf(m, "\trx_slen=%d,\n", sport_dev->rx_sub_len);
	ret += seq_printf(m, "\trx_rcv=%d\n", sport_dev->rx_received);
	ret += seq_printf(m, "\trx_total=%lu\n", sport_dev->rx_total);
	ret += seq_printf(m, "\trx_int_total=%lu\n", sport_dev->rx_int_total);
	ret += seq_printf(m, "\trx_overflow=%lu\n", sport_dev->rx_overflow);

	ret += seq_printf(m, "\ttx_len=%d,\n", sport_dev->tx_len);
	ret += seq_printf(m, "\ttx_buf_len=%d,\n", sport_dev->tx_buf_len);
	ret += seq_printf(m, "\ttx_buf_int_len=%d,\n", sport_dev->tx_buf_int_len);
	ret += seq_printf(m, "\ttx_slen=%d,\n", sport_dev->tx_sub_len);
	ret += seq_printf(m, "\ttx_sent=%d\n", sport_dev->tx_sent);
	ret += seq_printf(m, "\ttx_int_total=%lu\n", sport_dev->tx_int_total);
	ret += seq_printf(m, "\ttx_underflow=%lu\n", sport_dev->tx_underflow);
	
	ret += seq_printf(m, "\tcnt_rx=%d\n", sport_dev->count_rx_sub);
	ret += seq_printf(m, "\tcnt_tx=%d\n", sport_dev->count_tx_sub);	
	
	ret += seq_printf(m, "\tdelay_rx=%d\n", sport_dev->delay_rx_sub);
	ret += seq_printf(m, "\tdelay_tx=%d\n", sport_dev->delay_tx_sub);
	
#ifdef UPDATE_MAX_SAMPLES
	
	count = sport_dev->rx_max_len;
	if (count > sport_dev->tx_max_len)
		count = sport_dev->tx_max_len;

	if (sport_dev->rx_max && sport_dev->tx_max)
	{
		for (j=0;j<count;j++)
		{
			ret += seq_printf(m, "\trx_max[%d] = %ld\n", j, sport_dev->rx_max[j]);
			ret += seq_printf(m, "\ttx_max[%d] = %ld\n", j, sport_dev->tx_max[j]);
			
			sport_dev->tx_max[j] = 0;
			sport_dev->rx_max[j] = 0;
		}
	}
#endif
		
	return ret;
}

static int bfin_sport_ex_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, bfin_sport_ex_proc_show, NULL);
}

static const struct file_operations bfin_sport_ex_proc_fops = {
  .owner = THIS_MODULE,
  .open = bfin_sport_ex_proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};

#endif
/* -------------------------------------------------------------------------- */
static void rx_tsk_process(unsigned long data)
{
	struct s_sport_dev *sport_dev = (struct s_sport_dev *)data;
	if (sport_dev && sport_dev->in_use)
		sport_read(sport_dev);
}
/* -------------------------------------------------------------------------- */
static void tx_tsk_process(unsigned long data)
{
	struct s_sport_dev *sport_dev = (struct s_sport_dev *)data;
	if (sport_dev && sport_dev->in_use)
		sport_write(sport_dev);
}
/* -------------------------------------------------------------------------- */
void sport_deinit(void* context)
{
	struct s_sport_dev *sport_dev = &sport_device;
	sport_release(sport_dev);

#ifdef CONFIG_PROC_FS
	remove_proc_entry(SPORT_DRVNAME, NULL);
#endif

	sysclk_source_remove(sport_dev->sysclk_id);
}
/* -------------------------------------------------------------------------- */
int sport_init(void* context)
{
	int result;
	struct s_sport_dev *sport_dev = &sport_device;
	struct sport_stream_config_profile *stream_config = &sport_stream_config_list[PROFILE_ID];
	struct sport_config *config = &sport_config_list[PROFILE_ID];
	
	sport_dev->sysclk_id = sysclk_source_add(&sport_dev->sysclk_config);
	if (sport_dev->sysclk_id < 0)
		pr_err("sysclk_source_add: drvname='%s', error=%d\n", SPORT_DRVNAME, sport_dev->sysclk_id);
	
	result = sport_open(sport_dev);
	if (!result)
	{
		result = sport_stream_configure(sport_dev, &(stream_config->config));
		if (!result)
		{
			result = sport_configure(sport_dev, config);
			if (!result)
			{
				/**/
			}
			else
			{
				sport_release(sport_dev);
			}
		}
		else
		{
			sport_release(sport_dev);
		}
	}
	
#ifdef CONFIG_PROC_FS
	proc_create(SPORT_DRVNAME, 0444, NULL, &bfin_sport_ex_proc_fops);		
#endif

	return result;
}
/* -------------------------------------------------------------------------- */
