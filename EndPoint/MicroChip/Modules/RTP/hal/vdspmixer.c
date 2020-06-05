
#include "driver.h"
#include "voice-ioctl.h"

#define MIXER_CLASSNAME				"kvoice_mixer"

static struct file_operations mixer_fops;
static int				mixer_major	= 0;
static struct class*	mixer_class	= NULL;
static struct device*	mixer_device= NULL;

/*--------------------------------------------------------------------------*/
static void mixer_sync_single(vocoder_t* v, uint mixer)
{
}
/*- End of function --------------------------------------------------------*/
static void mixer_sync_input(vocoder_t* v, uint mixer, uint input)
{
}
/*- End of function --------------------------------------------------------*/
static int mixer_mute_single(vocoder_t* v, uint mixer)
{
	int j;
	if(mixer >= v->n_mixers) return -ENXIO;
	for(j=0; j < v->n_inputs;++j) {
		v->gain[mixer][j]=0;
	}
	mixer_sync_single(v, mixer);
	return 0;
}
/*- End of function --------------------------------------------------------*/
void mixer_mute_all(vocoder_t* v)
{
	int i;
	for(i=0; i < v->n_mixers; ++i)
		mixer_mute_single(v, i);
}
/*- End of function --------------------------------------------------------*/
void mixer_sync_all(vocoder_t* v)
{
	int i;
	for(i=0; i < v->n_mixers; ++i)
		mixer_sync_single(v, i);
}
/*- End of function --------------------------------------------------------*/
void mixer_restore(struct vocoder* v)
{
//	DPRINTK("\n");
	mixer_sync_all(v);
//	echo_sync_all(v);
}
/*- End of function --------------------------------------------------------*/
static int mixer_add_input(vocoder_t* v, uint mixer, uint input, u16 gain)
{
	if(mixer>=v->n_mixers) return -ENXIO;
	if (input < 0) return -ENOSPC;
	if (input >= v->n_inputs) return -ENOSPC;
	/* assign input */
	v->gain[mixer][input]=gain;
	mixer_sync_input(v, mixer, input);
	DPRINTK("success: mixer=%d, input=%d\n", mixer, input);
	return 0;
}
/*- End of function --------------------------------------------------------*/
static int mixer_remove_input(vocoder_t* v, uint mixer, uint input)
{
	if(mixer>=v->n_mixers) return -ENXIO;
	if (input < 0) return -ENOSPC;
	if (input >= v->n_inputs) return -ENOSPC;
	/* clear input */
	v->gain[mixer][input]=0;
	mixer_sync_input(v, mixer, input);
	DPRINTK("success\n");
	return 0;
}
/*- End of function --------------------------------------------------------*/
static int mixer_ioctl_tdm(struct file *file, struct mix_slot *ios)
{
	int ts;
	vocoder_t* v;
	v=vocoder_find(ios->dev_no);
	if(!v) return -ENODEV;
	if(ios->type != slot_tdm)  return -EINVAL;
	if(ios->number > v->n_mixers) return -ENOENT;
	ts = dsp_tdm_base(v->dsp)+ios->number;
	return ts;
}
/*- End of function --------------------------------------------------------*/
static int mixer_ioctl_enum_devs(struct file *file, struct mix_enum *ios)
{
	vocoder_t* v;
	++ios->number;
	while(ios->number <= dsp_get_last_number())
	{
		v = vocoder_find(ios->number);
		if(v) {
			if (v->n_mixers == 0) {
				++ios->number;
				continue;
			}
			DPRINTK("found %d\n", ios->number);
			ios->tdm_channels = v->n_mixers;
			ios->packet_channels = 0;
			ios->channels_per_conf = v->n_inputs;
			return 0;
		}
		++ios->number;
	}
	return -ENODEV;
}
/*- End of function --------------------------------------------------------*/
static __inline__ int mix_check(vocoder_t *v, struct mix_slot *m)
{
	if(m->number >= v->n_mixers) goto fail;
	if(m->type==slot_tdm)
		return m->number;
//	if(m->type==slot_packet)
//		return m->number + v->n_mixers/2;
fail:	
	DPRINTK("fail\n");
	return -EINVAL;
}
/*- End of function --------------------------------------------------------*/
static int mixer_ioctl_add(struct file *file, struct mix_slot_pair  *ios)
{
	vocoder_t* v;
	int mixer;
	int input;
	int result;
	int ts;
	
	DPRINTK("mixer(%d,%d,%s) <- input(%d,%d)\n",
		ios->mixer.dev_no, ios->mixer.number, ios->mixer.type ? "packet":"tdm",
		ios->input.number, ios->gain);

	v=vocoder_find(ios->mixer.dev_no);
	if(!v) return -ENODEV;
	
	mixer=mix_check(v, &ios->mixer);
	if(mixer < 0) return mixer;
	input=mix_check(v, &ios->input);
	if(input < 0) return input;
	
	result = mixer_add_input(v, mixer, input, ios->gain);
	if (result < 0) {
		DPRINTK("fail\n");
		return result;
	}
	if(ios->mixer.type==slot_tdm)
	{
		ts = mixer+dsp_tdm_base(v->dsp);
		return ts;
	}
	else
		return 0;
}
/*- End of function --------------------------------------------------------*/
static int mixer_ioctl_remove(struct file *file, struct mix_slot_pair *ios)
{
	vocoder_t *v;
	int mixer;
	int input;
	
	CHECK_PARAM(ios, VERIFY_READ);
	DPRINTK("mixer(%d,%d,%s) x input(%d)\n",
		ios->mixer.dev_no, ios->mixer.number, ios->mixer.type ? "packet":"tdm",
		ios->input.number);

	v=vocoder_find(ios->mixer.dev_no);
	if(!v) return -ENODEV;

	mixer=mix_check(v, &ios->mixer);
	if (mixer < 0) return mixer;
	input=mix_check(v, &ios->input);
	if (input < 0) return input;

	return mixer_remove_input(v, mixer, input);
}
/*- End of function --------------------------------------------------------*/
static int mixer_ioctl_clear(struct file *file, struct mix_slot *ios)
{
	vocoder_t *v;
	int mixer;
	
	CHECK_PARAM(ios, VERIFY_READ);
	DPRINTK("mixer(%d,%d,%s)\n", ios->dev_no, ios->number, ios->type ? "packet":"tdm");
	
	v=vocoder_find(ios->dev_no);
	if(!v) return -ENODEV;

	mixer=mix_check(v, ios);
	if(mixer < 0) return mixer;
	return mixer_mute_single(v, mixer);
}
/*- End of function --------------------------------------------------------*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int mixers_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#else
static long mixers_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
	switch (cmd) {
		case MIX_RX_CLEAR:	return mixer_ioctl_clear(file, (struct mix_slot*) arg);
		case MIX_RX_ADD:	return mixer_ioctl_add(file, (struct mix_slot_pair*) arg);
		case MIX_RX_REMOVE:	return mixer_ioctl_remove(file, (struct mix_slot_pair*) arg);
		case MIX_TDM:		return mixer_ioctl_tdm(file, (struct mix_slot*) arg);
		case MIX_ENUM_DEVS:	return mixer_ioctl_enum_devs(file, (struct mix_enum*) arg);
	}
	return -EINVAL;
}
/*- End of function --------------------------------------------------------*/
static int mixers_open(struct inode *inode, struct file *flip) {return 0;}
static int mixers_close(struct inode *node, struct file *flip) {return 0;}

static struct file_operations mixer_fops = {
	open: mixers_open,
	release: mixers_close,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	ioctl: mixers_ioctl,
#else
	unlocked_ioctl: mixers_ioctl,
	compat_ioctl: mixers_ioctl,
#endif
};

/*- End of function --------------------------------------------------------*/
int init_mixers(void)
{
	mixer_major = register_chrdev(0, MIXER_DEVNAME, &mixer_fops);
	if (mixer_major < 0)
	{
		printk(KERN_DEBUG  "failed to register device \"" MIXER_DEVNAME  "\" with error %d\n", mixer_major);
		return -EBUSY;
	}
	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	mixer_class = class_create(THIS_MODULE, MIXER_CLASSNAME);
	if (IS_ERR(mixer_class))
	{
		unregister_chrdev(mixer_major, MIXER_DEVNAME);
		printk(KERN_ALERT "Failed to register device class '%s'\n", MIXER_CLASSNAME);
		return PTR_ERR(mixer_class);
	}

	mixer_device = device_create(mixer_class, NULL, MKDEV(mixer_major, 0), NULL, MIXER_DEVNAME);
	if (IS_ERR(mixer_device))
	{
		class_destroy(mixer_class);
		unregister_chrdev(mixer_major, MIXER_DEVNAME);
		printk(KERN_ALERT "Failed to create the device '%s'\n", MIXER_DEVNAME);
		
		return PTR_ERR(mixer_device);
	}
#endif
	
	return 0;
}
/*- End of function --------------------------------------------------------*/
void cleanup_mixers(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	device_destroy(mixer_class, MKDEV(mixer_major, 0));
	class_unregister(mixer_class);
	class_destroy(mixer_class);
#endif
	unregister_chrdev(mixer_major, MIXER_DEVNAME);
}
/*- End of function --------------------------------------------------------*/
