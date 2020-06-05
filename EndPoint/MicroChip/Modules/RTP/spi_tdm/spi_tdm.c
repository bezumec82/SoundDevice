#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include "workspace.h"
#include "spi_tdm.h"
#include "hal.h"
#include "channel.h"
#include "vcodec.h"
#include "codec.h"
#include "voice-ioctl.h"
#include "spi-tdm-ioctl.h"

#include VERSION_INCLUDE

#ifdef KBUILD_MODNAME
#undef KBUILD_MODNAME
#endif

#if defined(HAL_GW60)

#define KBUILD_MODNAME		"spi_tdm6x"
#define CPUNAME				"BF60X"
#include "bf6xx_sport_ex.h"

#elif defined(HAL_GW50)

#define KBUILD_MODNAME		"spi_tdm5x"
#define CPUNAME				"BF52X"
#include "bf5xx_sport_ex.h"

#elif defined(HAL_GW40)

#define KBUILD_MODNAME		"spi_tdm4x"

#elif defined(HAL_GW30)

#define KBUILD_MODNAME		"spi_tdm3x"

#elif defined(HAL_GW20)

#define KBUILD_MODNAME		"spi_tdm2x"

#elif defined(HAL_GW10)

#define KBUILD_MODNAME		"spi_tdm1x"

#endif

#define SPI_TDM_CLASSNAME 	"spi_tdm_class"

MODULE_AUTHOR("q2l");
MODULE_DESCRIPTION("LEMZGW driver for SPORT");
MODULE_LICENSE("GPL");

#ifdef DPRINTK
#undef DPRINTK
#endif
#define DPRINTK(fmt, args...) printk("%s: " fmt, __FUNCTION__ , ## args)


#define DEF_PACKET_FACTOR	2
#define MAX_PACKET_FACTOR	8

/* jitter buffer default settings */
#define UPPER_WATERMARK			10
#define UPPER_DROP_WATERMARK	8
#define LOWER_DROP_WATERMARK	3
#define LOWER_WATERMARK			1

static int						num_of_spi_tdm = (SPI_TDM_LAST_CHANNEL-SPI_TDM_FIRST_CHANNEL+1);
static struct s_spi_tdm_device*	spi_tdm_devices[SPI_TDM_MAX_CHANNELS];
static struct list_head			spi_tdm_list;
static struct list_head			spi_tdm_free_list;
static short mux_buff[LIN16_DSP_FRAME_SIZE*MAX_PACKET_FACTOR];
static short demux_buff[LIN16_DSP_FRAME_SIZE*MAX_PACKET_FACTOR];

static struct file_operations spi_tdm_device_fops;
static int spi_tdm_major			= 0;
static struct class*  spi_tdm_class	= NULL;
static struct device* spi_tdm_dev	= NULL;

/* Init */
/*--------------------------------------------------------------------------*/
static inline struct list_head *get_free_list(void)
{
	return &spi_tdm_free_list;
}
/*--- End of function ------------------------------------------------------*/
struct s_spi_tdm_device* spi_tdm_create_new(int spi_tdm_channel)
{
	struct s_spi_tdm_device* s;

	if (!(s = kmalloc(sizeof(*s), GFP_KERNEL)))
		return 0;

	memset(s, 0, sizeof(*s));

	s->link = NULL;
	s->spi_tdm_channel = spi_tdm_channel;
	s->in_use = 0;
	s->timeslot = 0;

	list_add_tail(&s->list, &spi_tdm_list);
	spi_tdm_devices[spi_tdm_channel] = s;

	return s;
}
/*--- End of function ------------------------------------------------------*/
static void spi_tdm_destroy_all(void)
{
	struct s_spi_tdm_device *s;
	struct list_head *entry;

	while (!list_empty(&spi_tdm_list))
	{
		entry = spi_tdm_list.next;
		list_del(entry);
		s = list_entry(entry, struct s_spi_tdm_device, list);
		kfree(s);
	}
}
/*--- End of function ------------------------------------------------------*/
static int spi_tdm_ctl_init(void* context)
{
	struct s_spi_tdm_device *s;
	int i, spi_tdm_channel;
	spi_tdm_channel = SPI_TDM_FIRST_CHANNEL;
	
	INIT_LIST_HEAD(&spi_tdm_list);
	INIT_LIST_HEAD(&spi_tdm_free_list);
	
	for (i=0; i<num_of_spi_tdm; ++i, ++spi_tdm_channel)
	{
		if (!(s = spi_tdm_create_new(spi_tdm_channel)))
		{
			spi_tdm_destroy_all();
			return -ENOMEM;
		}
		DPRINTK("created %d spi tdm channel\n", spi_tdm_channel);
		list_add_tail(&s->free_list, &spi_tdm_free_list);
	}
	return 0;
}
/*--- End of function ------------------------------------------------------*/
static void spi_tdm_ctl_deinit(void* context)
{
	INIT_LIST_HEAD(&spi_tdm_free_list);
	spi_tdm_destroy_all();
}
/*--- End of function ------------------------------------------------------*/
/* API */
/*--------------------------------------------------------------------------*/
int spi_tdm_allocate(const struct s_spi_tdm_params *params, struct s_spi_tdm_device **spi_tdm_out)
{
	struct list_head *freelist;
	struct list_head *free;
	struct s_spi_tdm_device *s;

	*spi_tdm_out = 0;

	if (params->spi_tdm_channel < 0)
	{
		freelist = get_free_list();

		if (list_empty(freelist))
		{
			return -ENODEV;
		}
		free = freelist->next;
		s = list_entry(free, struct s_spi_tdm_device, free_list);
	}
	else
	{
		if (params->spi_tdm_channel >= SPI_TDM_MAX_CHANNELS)
			return -ENODEV;

		s = spi_tdm_devices[params->spi_tdm_channel];

		if (!s)
			return -ENODEV;
			
		if (s->in_use)
		{
			return -EBUSY;
		}
		free = &s->free_list;
		freelist = get_free_list();
	}

	s->context = params->context;
	s->in_use = 1;
	list_del(free);
	INIT_LIST_HEAD(free);

	s->owner[0] = '\0';

	*spi_tdm_out = s;
	return 0;
}
/*--- End of function ------------------------------------------------------*/
void spi_tdm_release(struct s_spi_tdm_device *s)
{
	struct list_head *freelist = get_free_list();
	if(s == NULL) return;	

	s->context = 0;
	list_add(&s->free_list, freelist);
	s->in_use = 0;
}
/*--- End of function ------------------------------------------------------*/
int spi_tdm_timeslot(struct s_spi_tdm_device *s)
{
	return s->timeslot;
}
/*--- End of function ------------------------------------------------------*/
int spi_tdm_num_of_channels(void)
{
	return num_of_spi_tdm;
}
/*--- End of function ------------------------------------------------------*/
struct s_spi_tdm_device *spi_tdm_get_channel(int spi_tdm_channel)
{
	struct s_spi_tdm_device *s = NULL;

	if ((spi_tdm_channel >= 0) && (spi_tdm_channel < SPI_TDM_MAX_CHANNELS))
		s = spi_tdm_devices[spi_tdm_channel];
	
	return s;
}
/*--- End of function ------------------------------------------------------*/
int spi_tdm_set_owner(struct s_spi_tdm_device *s, char* owner)
{
	int len;
	if (s == NULL) return 0;
	len = strlen(owner);
	if (len > sizeof(s->owner)) len=sizeof(s->owner);
	memcpy(s->owner, owner, len);
	return len;
}
/*--- End of function ------------------------------------------------------*/
int spi_tdm_get_owner(struct s_spi_tdm_device *s, char* owner)
{
	int len;
	if (s == NULL) return 0;
	len=strlen(s->owner);
	memcpy(owner, s->owner, len);
	return len;
}
/*--- End of function ------------------------------------------------------*/
void inline bind_channel(struct s_spi_tdm_device *s, channel_t* c)
{
	s->link = c;
	c->spi_tdm	= s;

	c->decoder_upper_watermark		= s->code_frame_size * UPPER_WATERMARK;
	c->decoder_upper_drop_watermark	= s->code_frame_size * UPPER_DROP_WATERMARK;
	c->decoder_lower_drop_watermark	= s->code_frame_size * LOWER_DROP_WATERMARK;
	c->decoder_lower_watermark		= s->code_frame_size * LOWER_WATERMARK;

	c->ufw = 0;
	c->ufw = 0;
	
	spi_tdm_set_owner(s, c->devname);
}
/*--- End of function ------------------------------------------------------*/
void inline unbind_channel(struct s_spi_tdm_device *s, channel_t* c)
{
	if (c==NULL) {
		printk("attempt to release NULL channel\n");
		return;
	}
	s->link = NULL;
	c->spi_tdm = NULL;
	spi_tdm_set_owner(s, "");
	DPRINTK("deinit spi tdm device for channel %d\n", c->tdm);
	free_channel(c);
}
/*--- End of function ------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Reading data from channel fifo convert to TDM */
int spi_tdm_mux(int channels_count, int channel_size, void *pmux)
{
	struct s_spi_tdm_device*	s;
	channel_t* c;
 	short *poutput16 = (short *)pmux;
 	short *pinput16;
 	int nread = 0;
 	int nsamples = 0;
 	int mux_size = 0;
 	int payloadtype = 0; 	
 	int k,i,j;
 	
	if (poutput16 && (channels_count > 0) && (channel_size > 0))
	{
		memset(pmux, 0, channel_size*channels_count);
		mux_size = sizeof(mux_buff);
		if (mux_size > channel_size) mux_size = channel_size;
		nsamples = mux_size / 2; /* div sizeof element */
		
		for (k=0;k<SPI_TDM_MAX_CHANNELS;k++)
		{
			s = spi_tdm_devices[k];
			if (s && s->in_use && s->spi_tdm_channel < channels_count)
			{
				c = s->link;
				nread = c->read_enc_frame(c, &payloadtype, mux_buff, mux_size);
				pinput16 = mux_buff;
				for (j=s->spi_tdm_channel,i=0;i<nsamples;i++,j+=channels_count)
				{
					poutput16[j] = pinput16[i];
				}
			}
		}
	}

	return mux_size;
}
/*----------------------------------------------------------------------------*/
/* Reading data from TDM convert to channel fifo */
int spi_tdm_demux(int channels_count, int channel_size, void *pdemux)
{
	struct s_spi_tdm_device*	s;
	channel_t* c;
 	short *poutput16;
 	short *pinput16 = (short *)pdemux;
 	int demux_size = 0;
 	int nsamples = 0;
 	int nwrite = 0;
 	int i,j,k;
 	
	if (pinput16 && (channels_count > 0) && (channel_size > 0))
	{
        ///@bug
		///memset(pdemux, 0, channel_size*channels_count);
		demux_size = sizeof(demux_buff);
		if (demux_size > channel_size) demux_size = channel_size;
		nsamples = demux_size / 2; /* div sizeof element */
		
		for (k=0;k<SPI_TDM_MAX_CHANNELS;k++)
		{
			s = spi_tdm_devices[k];
			if (s && s->in_use && s->spi_tdm_channel < channels_count)
			{
				c = s->link;
				poutput16 = demux_buff;
				for (j=s->spi_tdm_channel,i=0;i<nsamples;i++,j+=channels_count)
				{
					poutput16[i] = pinput16[j];
				}
				nwrite = c->write_dec_frame(c, PAYLOAD_TYPE_LIN16, demux_buff, demux_size);
			}
		}
	}
	return demux_size;
}
/*----------------------------------------------------------------------------*/
static int spi_tdm_device_allocate(struct s_spi_tdm_params *params)
{
	channel_t* c=NULL;
	struct s_spi_tdm_device *s=NULL;
	int error;
	unsigned int caps;
	int	pcm_frame_size=0;
	int	code_frame_size=0;
	
	if (params->packet_factor <= 0) params->packet_factor	= DEF_PACKET_FACTOR;
	if (params->packet_factor > MAX_PACKET_FACTOR) params->packet_factor	= MAX_PACKET_FACTOR;

	/* precalc code_frame_size for TX process */
	switch (params->rx_pt) {
		case PAYLOAD_TYPE_LIN16:
			code_frame_size = params->packet_factor * LIN16_DSP_FRAME_SIZE;
			pcm_frame_size = params->packet_factor * LIN16_PCM_FRAME_SIZE;
		break;
	}

	
	caps = caps_from_pt(params->rx_pt, params->tx_pt);
	DPRINTK("spi tdm device init: PT(rx/tx)=%d/%d. caps=%08x\n", params->rx_pt, params->tx_pt, caps);
	
	if (params->tdm >= 0) {
		c = get_channel_tdm(params->tdm, caps);
		if(c==NULL)	{
			DPRINTK("fail to get channel on tdm=%d\n", params->tdm);
			return -ENODEV;
		}
		goto done;
	}
	
	c = alloc_channel(caps);
	if(c==NULL)	{
		DPRINTK("cannot allocate channel\n");
		return -ENODEV;
	}

done:

	params->context=c;
	
	error = spi_tdm_allocate(params, &s);
	if (error<0)
	{
		free_channel(c);
		return error;
	}
	
	s->param = *params;
	s->pcm_frame_size = pcm_frame_size;
	s->code_frame_size = code_frame_size;

	bind_channel(s, c);

	DPRINTK("return tdm=%d\n", c->tdm);
	
	return 0;
}
/*--- End of function ------------------------------------------------------*/
int spi_tdm_device_free(struct s_spi_tdm_device *s)
{
	channel_t* c=NULL;
	if(s==NULL)	{
		DPRINTK("cannot free spi tdm device\n");
		return -ENODEV;
	}
	
	c = s->link;
	
	if(c==NULL)	{
		DPRINTK("cannot free channel\n");
		return -ENODEV;
	}	

	unbind_channel(s, c);

	return 0;
}
/*--- End of function ------------------------------------------------------*/
int spi_tdm_device_init(void *context)
{
	spi_tdm_major = register_chrdev(0, SPI_TDM_DEVNAME, &spi_tdm_device_fops);
	
	if (spi_tdm_major < 0)
	{
		printk(KERN_DEBUG  "failed to register device \"" SPI_TDM_DEVNAME "\" with error %d\n", spi_tdm_major);
		return -EBUSY;
	}
	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	spi_tdm_class = class_create(THIS_MODULE, SPI_TDM_CLASSNAME);
	if (IS_ERR(spi_tdm_class))
	{
		unregister_chrdev(spi_tdm_major, SPI_TDM_DEVNAME);
		printk(KERN_ALERT "Failed to register device class '%s'\n", SPI_TDM_CLASSNAME);
		return PTR_ERR(spi_tdm_class);
	}

	spi_tdm_dev = device_create(spi_tdm_class, NULL, MKDEV(spi_tdm_major, 0), NULL, SPI_TDM_DEVNAME);
	if (IS_ERR(spi_tdm_dev))
	{
		class_destroy(spi_tdm_class);
		unregister_chrdev(spi_tdm_major, SPI_TDM_DEVNAME);
		printk(KERN_ALERT "Failed to create the device '%s'\n", SPI_TDM_DEVNAME);
		
		return PTR_ERR(spi_tdm_dev);
	}
#endif

	return 0;
}
/*--- End of function ------------------------------------------------------*/
void spi_tdm_device_deinit(void *context)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	device_destroy(spi_tdm_class, MKDEV(spi_tdm_major, 0));
	class_unregister(spi_tdm_class);
	class_destroy(spi_tdm_class);
#endif

	unregister_chrdev(spi_tdm_major, SPI_TDM_DEVNAME);
}
/*--- End of function ------------------------------------------------------*/
static int spi_tdm_device_open(struct inode *inode, struct file *file)
{
	file->private_data = spi_tdm_devices;
	return 0;
}
/*--- End of function ------------------------------------------------------*/
static int spi_tdm_device_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	return 0;
}
/*--- End of function ------------------------------------------------------*/
static int spi_tdm_device_add_channel(struct file *file, struct s_spi_tdm_channel_info* info)
{
	struct s_spi_tdm_params params;
	int result = -EINVAL;
	
	if (info)
	{
		params.spi_tdm_channel	= info->spi_tdm_channel;
		params.tdm				= info->tdm;
		params.packet_factor	= SPI_TDM_PACKET_FACTOR;
		params.rx_pt			= PAYLOAD_TYPE_LIN16;
		params.tx_pt			= PAYLOAD_TYPE_LIN16;
		params.context			= NULL;
		printk("spi_tdm_device_add_channel 1\n");
		result = spi_tdm_device_allocate(&params);
		printk("spi_tdm_device_add_channel 2\n");
	}
	return result;
}
/*--- End of function ------------------------------------------------------*/
static int spi_tdm_device_free_channel(struct file *file, struct s_spi_tdm_channel_info* info)
{
	struct s_spi_tdm_device *s = NULL;
	int result = -EINVAL;

	if (info)
	{
		s = spi_tdm_get_channel(info->spi_tdm_channel);
		result = spi_tdm_device_free(s);
	}
	return result;
}
/*--- End of function ------------------------------------------------------*/
static int spi_tdm_device_get_stat_info(struct file *file, struct s_spi_tdm_stat_info* info)
{
	struct s_spi_tdm_device *s = NULL;
	int result = -EINVAL;
	if (info)
	{
		s = spi_tdm_get_channel(info->spi_tdm_channel);
		if (s)
		{
			info->in_use	= s->in_use;
			info->tdm		= -1;
			if (s->link)
				info->tdm		= s->link->tdm;	
			result = 0;
		}
		else
		{
			result = -ENODEV;
		}
	}
	return result;
}
/*--- End of function ------------------------------------------------------*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
int spi_tdm_device_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#else
long spi_tdm_device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
{

	switch (cmd)
	{
		case SPI_TDM_ADD_CHANNEL:		return spi_tdm_device_add_channel(file, (struct s_spi_tdm_channel_info*) arg);
		case SPI_TDM_REMOVE_CHANNEL:	return spi_tdm_device_free_channel(file, (struct s_spi_tdm_channel_info*) arg);
		case SPI_TDM_STAT_INFO:			return spi_tdm_device_get_stat_info(file, (struct s_spi_tdm_stat_info*) arg);
	}
	return -EINVAL;
}
/*--- End of function ------------------------------------------------------*/
static struct file_operations spi_tdm_device_fops = {
	open   : spi_tdm_device_open,
	release: spi_tdm_device_release,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	ioctl  : spi_tdm_device_ioctl
#else
	unlocked_ioctl:spi_tdm_device_ioctl,
	compat_ioctl:spi_tdm_device_ioctl,
#endif	
};
/*--- End of function ------------------------------------------------------*/

/* Module */
/*--------------------------------------------------------------------------*/
static char s_spi_tdm_version[256] = "";
const char *spi_tdm_version( void )
{
	if( !s_spi_tdm_version[0] )
	{
		snprintf( s_spi_tdm_version, sizeof(s_spi_tdm_version), 
		"%s%s: driver for SPORT Blackfin %s\n"
			  "%s: %s\n", 
		KERN_INFO, KBUILD_MODNAME, CPUNAME, KBUILD_MODNAME, version_string() );
	}
	return s_spi_tdm_version;
}

int spi_tdm_context;
char *init[MAX_ENABLE_STRS] = {[0 ... MAX_ENABLE_STRS - 1] = NULL };

INIT_PROLOGUE(spi_tdm)
	INIT_PART_DEFAULT(spi_tdm_device)
	INIT_PART_DEFAULT(spi_tdm_ctl)
	INIT_PART_DEFAULT(sport)
INIT_EPILOGUE

static int mod_init(void)
{
	static int version_printed = 0;

	if (version_printed++ == 0) {
		printk(spi_tdm_version());
	}

	return init_startup (
                        &spi_tdm_context, 
                        INIT_STRUCT_NAME(spi_tdm), 
                        INIT_NUM_ITEMS(spi_tdm), 
                        init, 
                        MAX_ENABLE_STRS
                        );
}

static void mod_cleanup(void)
{
	init_shutdown(&spi_tdm_context, INIT_STRUCT_NAME(spi_tdm), INIT_NUM_ITEMS(spi_tdm));
}

module_init(mod_init)
module_exit(mod_cleanup)

EXPORT_SYMBOL(spi_tdm_mux);
EXPORT_SYMBOL(spi_tdm_demux);

/*--- End of file ----------------------------------------------------------*/
