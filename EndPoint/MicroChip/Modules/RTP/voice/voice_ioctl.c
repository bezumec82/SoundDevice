#include "global.h"

#define VOICE_CLASSNAME				"kvoice_voice"

static struct file_operations voice_fops;
static int				voice_major	= 0;
static struct class*	voice_class	= NULL;
static struct device*	voice_device= NULL;

extern void module_inc_use_count(void);
extern void module_dec_use_count(void);

/*- End of function --------------------------------------------------------*/
int voice_ctl_init(void *context)
{
	voice_major = register_chrdev(0, VOICE_DEVNAME, &voice_fops);
	if (voice_major < 0)
	{
		printk(KERN_DEBUG  "failed to register device \"" VOICE_DEVNAME  "\" with error %d\n", voice_major);
		return -EBUSY;
	}
	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	voice_class = class_create(THIS_MODULE, VOICE_CLASSNAME);
	if (IS_ERR(voice_class))
	{
		unregister_chrdev(voice_major, VOICE_DEVNAME);
		printk(KERN_ALERT "Failed to register device class '%s'\n", VOICE_CLASSNAME);
		return PTR_ERR(voice_class);
	}

	voice_device = device_create(voice_class, NULL, MKDEV(voice_major, 0), NULL, VOICE_DEVNAME);
	if (IS_ERR(voice_device))
	{
		class_destroy(voice_class);
		unregister_chrdev(voice_major, VOICE_DEVNAME);
		printk(KERN_ALERT "Failed to create the device '%s'\n", VOICE_DEVNAME);
		
		return PTR_ERR(voice_device);
	}
#endif
	
	return 0;
}
/*- End of function --------------------------------------------------------*/
void voice_ctl_deinit(void *context)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	device_destroy(voice_class, MKDEV(voice_major, 0));
	class_unregister(voice_class);
	class_destroy(voice_class);
#endif
	unregister_chrdev(voice_major, VOICE_DEVNAME);
}
/*- End of function --------------------------------------------------------*/
static int voice_open(struct inode *inode, struct file *file)
{
	module_inc_use_count();
	return 0;
}
/*- End of function --------------------------------------------------------*/
static int voice_release(struct inode *inode, struct file *file)
{
	module_dec_use_count();
	return 0;
}
/*- End of function --------------------------------------------------------*/
static __inline__ int voice_install_codec(struct file *file, struct vocoder_install_param *ios)
{
	unsigned int dsp, codec_id;

	GET_PARAM(dsp, ios->dsp);
	GET_PARAM(codec_id, ios->codec_id);
	return vocoder_install(dsp, codec_id);
}
/*- End of function --------------------------------------------------------*/
static __inline__ int voice_uninstall_codec(struct file *file, unsigned int *ios)
{
	unsigned int dsp;

	GET_PARAM(dsp, *ios);
	return vocoder_uninstall(dsp);
}
/*- End of function --------------------------------------------------------*/
static int voice_num_vocoder_chns(struct file *file, unsigned int *ios)
{
	unsigned int voc_type;

	GET_PARAM(voc_type, *ios);
	return vocoder_get_num_channels(voc_type);
}
/*- End of function --------------------------------------------------------*/
static int voice_alloc_channel(struct file *file, unsigned int *ios)
{
	unsigned int voc_type;

	GET_PARAM(voc_type, *ios);
	return vocoder_alloc_channel(voc_type);
}
/*- End of function --------------------------------------------------------*/
static int voice_stat_channel(struct file *file, struct vocoder_stat_param *ios)
{
	struct channel_stat_param	info;
	int result;
	result = vocoder_stat_channel(ios->channel, &info);
	
	ios->decoder_fifo_stat.read_bytes	= info.decoder_fifo_stat.read_bytes;
	ios->decoder_fifo_stat.read_frames	= info.decoder_fifo_stat.read_frames;
	ios->decoder_fifo_stat.read_calls	= info.decoder_fifo_stat.read_calls;
	ios->decoder_fifo_stat.write_bytes	= info.decoder_fifo_stat.write_bytes;
	ios->decoder_fifo_stat.write_frames	= info.decoder_fifo_stat.write_frames;
	ios->decoder_fifo_stat.write_calls	= info.decoder_fifo_stat.write_calls;
	ios->decoder_fifo_stat.size			= info.decoder_fifo_stat.size;

	ios->encoder_fifo_stat.read_bytes	= info.encoder_fifo_stat.read_bytes;
	ios->encoder_fifo_stat.read_frames	= info.encoder_fifo_stat.read_frames;
	ios->encoder_fifo_stat.read_calls	= info.encoder_fifo_stat.read_calls;
	ios->encoder_fifo_stat.write_bytes	= info.encoder_fifo_stat.write_bytes;
	ios->encoder_fifo_stat.write_frames	= info.encoder_fifo_stat.write_frames;
	ios->encoder_fifo_stat.write_calls	= info.encoder_fifo_stat.write_calls;
	ios->encoder_fifo_stat.size			= info.encoder_fifo_stat.size;

	ios->ofw = info.ofw;
	ios->ufw = info.ufw;
	
	return result;
}
/*- End of function --------------------------------------------------------*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int voice_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#else
static long voice_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
	switch (cmd)
	{
		case VOCODER_INSTALL:		return voice_install_codec(file, (struct vocoder_install_param *)arg);
		case VOCODER_UNINSTALL:		return  voice_uninstall_codec(file, (unsigned int *)arg);
		case VOCODER_NUM_CHANNELS:	return  voice_num_vocoder_chns(file, (unsigned int *)arg);
		case VOCODER_ALLOC_CHANNEL:	return  voice_alloc_channel(file, (unsigned int *)arg);
		case VOCODER_STAT_CHANNEL:	return  voice_stat_channel(file, (struct vocoder_stat_param *)arg);
	}
	return -EINVAL;
}
/*- End of function --------------------------------------------------------*/
static struct file_operations voice_fops = {
	open: voice_open,
	release: voice_release,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	ioctl: voice_ioctl,
#else
    unlocked_ioctl:voice_ioctl,
    compat_ioctl:voice_ioctl,
#endif
};
/*--------------------------------------------------------------------------*/

