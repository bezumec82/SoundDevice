#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include "global.h"

#include VERSION_INCLUDE

MODULE_AUTHOR("draky j. void, q2l");
MODULE_DESCRIPTION("LEMZGW voice driver");
MODULE_LICENSE("GPL");

const char *voice_banner = "(voice: no version)\n";
const char *voice_author = "(voice: no author)\n";
static char s_banner[256] = "";
static char s_author[256] = "";

int vocoders_init(void *context);
void vocoders_deinit(void *context);

int voice_ctl_init(void *context);
void voice_ctl_deinit(void *context);

int rtp_ctl_init(void *context);
void rtp_ctl_deinit(void *context);

int rtp_ext_ctl_init(void *context);
void rtp_ext_ctl_deinit(void *context);

INIT_PROLOGUE(voice)
INIT_PART_DEFAULT(vocoders)
INIT_PART_DEFAULT(voice_ctl)
INIT_PART_DEFAULT(rtp_ctl)
INIT_PART_DEFAULT(rtp_ext_ctl)
INIT_EPILOGUE

/*--------------------------------------------------------------------------*/
static void init_version_information( void )
{
    if( !s_banner[0] )
    {
	snprintf( s_banner, sizeof(s_banner),
	    "%s%s: %s\n", KERN_INFO, VOICE_DEVNAME, version_string() );
	voice_banner = s_banner;
    }
    if( !s_author[0] )
    {
	snprintf( s_author, sizeof(s_author),
	    "%s%s: %s\n", KERN_INFO, VOICE_DEVNAME, version_copyright() );
	voice_author = s_author;
    }
}
/*- End of function --------------------------------------------------------*/
static int mod_init(void)
{
	static int version_printed = 0;
	int err;

	if (version_printed++ == 0) 
	{
		init_version_information();
		printk(voice_banner);
		printk(voice_author);
	}

	err = init_startup(NULL, INIT_STRUCT_NAME(voice), INIT_NUM_ITEMS(voice), NULL, 0);
	return err;
}
/*- End of function --------------------------------------------------------*/
static void mod_cleanup(void)
{
	init_shutdown(NULL, INIT_STRUCT_NAME(voice), INIT_NUM_ITEMS(voice));
}
/*- End of function --------------------------------------------------------*/
void module_inc_use_count(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	MOD_INC_USE_COUNT;
#endif
}
/*- End of function --------------------------------------------------------*/
void module_dec_use_count(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	MOD_DEC_USE_COUNT;
#endif
}
/*- End of function --------------------------------------------------------*/

#ifdef KBUILD_MODNAME
#undef KBUILD_MODNAME
#endif
#define KBUILD_MODNAME "kvoice"

module_init(mod_init)
module_exit(mod_cleanup)

