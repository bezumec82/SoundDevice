#include"mcasp_mod.h"

/*!
Test:
cd /sys/bus/platform/devices/f8004000.mcasp0/regs
grep -H . .
 */

/* Functions prototypes */
static ssize_t mcasp_store	(
							struct device * dev,
							struct device_attribute * attr,
							const char * buf,
							size_t count
							);
static ssize_t mcasp_show	(
							struct device * dev,
							struct device_attribute * attr,
							char * buf
							);
/*--- End of functions prototypes ----------------------------------------------------------------*/

/* Data declaration */
static struct device_attribute dev_attr_McASP_REV				= {.attr = { .name ="REV" ,				.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_PWRIDLESYSCONFIG	= {.attr = { .name ="PWRIDLESYSCONFIG",	.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_PFUNC				= {.attr = { .name ="PFUNC" ,			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_PDIR				= {.attr = { .name ="PDIR",				.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_PDOUT				= {.attr = { .name ="PDOUT",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_PDIN				= {.attr = { .name ="PDIN",				.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_PDCLR				= {.attr = { .name ="PDCLR",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_GBLCTL			= {.attr = { .name ="GBLCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
#if AMUTE_PRESENT
	static struct device_attribute dev_attr_McASP_AMUTE			= {.attr = { .name ="AMUTE",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
#endif
static struct device_attribute dev_attr_McASP_DLBCTL			= {.attr = { .name ="DLBCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
#if (DIT_MODE)
	static struct device_attribute dev_attr_McASP_DITCTL		= {.attr = { .name ="DITCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
#endif
static struct device_attribute dev_attr_McASP_RGBLCTL			= {.attr = { .name ="RGBLCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RMASK				= {.attr = { .name ="RMASK",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RFMT				= {.attr = { .name ="RFMT",				.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_AFSRCTL			= {.attr = { .name ="AFSRCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_ACLKRCTL			= {.attr = { .name ="ACLKRCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_AHCLKRCTL			= {.attr = { .name ="AHCLKRCTL",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RTDM				= {.attr = { .name ="RTDM",				.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RINTCTL			= {.attr = { .name ="RINTCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RSTAT				= {.attr = { .name ="RSTAT",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RSLOT				= {.attr = { .name ="RSLOT",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RCLKCHK			= {.attr = { .name ="RCLKCHK",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_REVTCTL			= {.attr = { .name ="REVTCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	
static struct device_attribute dev_attr_McASP_XGBLCTL			= {.attr = { .name ="XGBLCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XMASK				= {.attr = { .name ="XMASK",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XFMT				= {.attr = { .name ="XFMT",				.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_AFSXCTL			= {.attr = { .name ="AFSXCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_ACLKXCTL			= {.attr = { .name ="ACLKXCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_AHCLKXCTL			= {.attr = { .name ="AHCLKXCTL",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XTDM				= {.attr = { .name ="XTDM",				.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XINTCTL			= {.attr = { .name ="XINTCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XSTAT				= {.attr = { .name ="XSTAT",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XSLOT				= {.attr = { .name ="XSLOT",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XCLKCHK			= {.attr = { .name ="XCLKCHK",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XEVTCTL			= {.attr = { .name ="XEVTCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
#if (DIT_MODE)
	static struct device_attribute dev_attr_McASP_DITCSRA_0		= {.attr = { .name ="DITCSRA_0",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITCSRA_1		= {.attr = { .name ="DITCSRA_1",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITCSRA_2		= {.attr = { .name ="DITCSRA_2",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITCSRA_3		= {.attr = { .name ="DITCSRA_3",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITCSRA_4		= {.attr = { .name ="DITCSRA_4",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITCSRA_5		= {.attr = { .name ="DITCSRA_5",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
		
	static struct device_attribute dev_attr_McASP_DITCSRB_0		= {.attr = { .name ="DITCSRB_0",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITCSRB_1		= {.attr = { .name ="DITCSRB_1",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITCSRB_2		= {.attr = { .name ="DITCSRB_2",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITCSRB_3		= {.attr = { .name ="DITCSRB_3",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITCSRB_4		= {.attr = { .name ="DITCSRB_4",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITCSRB_5		= {.attr = { .name ="DITCSRB_5",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
		
	static struct device_attribute dev_attr_McASP_DITUDRA_0		= {.attr = { .name ="DITUDRA_0",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITUDRA_1		= {.attr = { .name ="DITUDRA_1",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITUDRA_2		= {.attr = { .name ="DITUDRA_2",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITUDRA_3		= {.attr = { .name ="DITUDRA_3",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITUDRA_4		= {.attr = { .name ="DITUDRA_4",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITUDRA_5		= {.attr = { .name ="DITUDRA_5",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
		
	static struct device_attribute dev_attr_McASP_DITUDRB_0		= {.attr = { .name ="DITUDRB_0",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITUDRB_1		= {.attr = { .name ="DITUDRB_1",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITUDRB_2		= {.attr = { .name ="DITUDRB_2",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITUDRB_3		= {.attr = { .name ="DITUDRB_3",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITUDRB_4		= {.attr = { .name ="DITUDRB_4",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
	static struct device_attribute dev_attr_McASP_DITUDRB_5		= {.attr = { .name ="DITUDRB_5",		.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
#endif
static struct device_attribute dev_attr_McASP_SRCTL_0			= {.attr = { .name ="SRCTL_0",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_SRCTL_1			= {.attr = { .name ="SRCTL_1",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_SRCTL_2			= {.attr = { .name ="SRCTL_2",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_SRCTL_3			= {.attr = { .name ="SRCTL_3",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_SRCTL_4			= {.attr = { .name ="SRCTL_4",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_SRCTL_5			= {.attr = { .name ="SRCTL_5",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
#if (CFG_BUS)		
static struct device_attribute dev_attr_McASP_XBUF_0			= {.attr = { .name ="XBUF_0",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XBUF_1			= {.attr = { .name ="XBUF_1",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XBUF_2			= {.attr = { .name ="XBUF_2",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XBUF_3			= {.attr = { .name ="XBUF_3",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XBUF_4			= {.attr = { .name ="XBUF_4",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_XBUF_5			= {.attr = { .name ="XBUF_5",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
																		
static struct device_attribute dev_attr_McASP_RBUF_0			= {.attr = { .name ="RBUF_0",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RBUF_1			= {.attr = { .name ="RBUF_1",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RBUF_2			= {.attr = { .name ="RBUF_2",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RBUF_3			= {.attr = { .name ="RBUF_3",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RBUF_4			= {.attr = { .name ="RBUF_4",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RBUF_5			= {.attr = { .name ="RBUF_5",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
#endif		
static struct device_attribute dev_attr_McASP_WFIFOCTL			= {.attr = { .name ="WFIFOCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_WFIFOSTS			= {.attr = { .name ="WFIFOSTS",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RFIFOCTL			= {.attr = { .name ="RFIFOCTL",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };
static struct device_attribute dev_attr_McASP_RFIFOSTS			= {.attr = { .name ="RFIFOSTS",			.mode = S_IRUGO | S_IWUSR, }, .show = mcasp_show, .store = mcasp_store, };

/* Declare attributes table (listing in fact sysfs files for driver) */
static struct attribute * mcasp_attrs[] = {
	&dev_attr_McASP_REV					.attr,
	&dev_attr_McASP_PWRIDLESYSCONFIG	.attr,
	&dev_attr_McASP_PFUNC				.attr,
	&dev_attr_McASP_PDIR				.attr,
	&dev_attr_McASP_PDOUT				.attr,
	&dev_attr_McASP_PDIN				.attr,
	&dev_attr_McASP_PDCLR				.attr,
	&dev_attr_McASP_GBLCTL				.attr,
#if AMUTE_PRESENT
	&dev_attr_McASP_AMUTE				.attr,
#endif
	&dev_attr_McASP_DLBCTL				.attr,
#if (DIT_MODE)
	&dev_attr_McASP_DITCTL				.attr,
#endif
	&dev_attr_McASP_RGBLCTL				.attr,
	&dev_attr_McASP_RMASK				.attr,
	&dev_attr_McASP_RFMT				.attr,
	&dev_attr_McASP_AFSRCTL				.attr,
	&dev_attr_McASP_ACLKRCTL			.attr,
	&dev_attr_McASP_AHCLKRCTL			.attr,
	&dev_attr_McASP_RTDM				.attr,
	&dev_attr_McASP_RINTCTL				.attr,
	&dev_attr_McASP_RSTAT				.attr,
	&dev_attr_McASP_RSLOT				.attr,
	&dev_attr_McASP_RCLKCHK				.attr,
	&dev_attr_McASP_REVTCTL				.attr,
	&dev_attr_McASP_XGBLCTL				.attr,
	&dev_attr_McASP_XMASK				.attr,
	&dev_attr_McASP_XFMT				.attr,
	&dev_attr_McASP_AFSXCTL				.attr,
	&dev_attr_McASP_ACLKXCTL			.attr,
	&dev_attr_McASP_AHCLKXCTL			.attr,
	&dev_attr_McASP_XTDM				.attr,
	&dev_attr_McASP_XINTCTL				.attr,
	&dev_attr_McASP_XSTAT				.attr,
	&dev_attr_McASP_XSLOT				.attr,
	&dev_attr_McASP_XCLKCHK				.attr,
	&dev_attr_McASP_XEVTCTL				.attr,
#if (DIT_MODE)
	&dev_attr_McASP_DITCSRA_0			.attr,
	&dev_attr_McASP_DITCSRA_1			.attr,
	&dev_attr_McASP_DITCSRA_2			.attr,
	&dev_attr_McASP_DITCSRA_3			.attr,
	&dev_attr_McASP_DITCSRA_4			.attr,
	&dev_attr_McASP_DITCSRA_5			.attr,

	&dev_attr_McASP_DITCSRB_0			.attr,
	&dev_attr_McASP_DITCSRB_1			.attr,
	&dev_attr_McASP_DITCSRB_2			.attr,
	&dev_attr_McASP_DITCSRB_3			.attr,
	&dev_attr_McASP_DITCSRB_4			.attr,
	&dev_attr_McASP_DITCSRB_5			.attr,

	&dev_attr_McASP_DITUDRA_0			.attr,
	&dev_attr_McASP_DITUDRA_1			.attr,
	&dev_attr_McASP_DITUDRA_2			.attr,
	&dev_attr_McASP_DITUDRA_3			.attr,
	&dev_attr_McASP_DITUDRA_4			.attr,
	&dev_attr_McASP_DITUDRA_5			.attr,

	&dev_attr_McASP_DITUDRB_0			.attr,
	&dev_attr_McASP_DITUDRB_1			.attr,
	&dev_attr_McASP_DITUDRB_2			.attr,
	&dev_attr_McASP_DITUDRB_3			.attr,
	&dev_attr_McASP_DITUDRB_4			.attr,
	&dev_attr_McASP_DITUDRB_5			.attr,
#endif
	&dev_attr_McASP_SRCTL_0				.attr,
	&dev_attr_McASP_SRCTL_1				.attr,
	&dev_attr_McASP_SRCTL_2				.attr,
	&dev_attr_McASP_SRCTL_3				.attr,
	&dev_attr_McASP_SRCTL_4				.attr,
	&dev_attr_McASP_SRCTL_5				.attr,
#if (CFG_BUS)
	&dev_attr_McASP_XBUF_0				.attr,
	&dev_attr_McASP_XBUF_1				.attr,
	&dev_attr_McASP_XBUF_2				.attr,
	&dev_attr_McASP_XBUF_3				.attr,
	&dev_attr_McASP_XBUF_4				.attr,
	&dev_attr_McASP_XBUF_5				.attr,

	&dev_attr_McASP_RBUF_0				.attr,
	&dev_attr_McASP_RBUF_1				.attr,
	&dev_attr_McASP_RBUF_2				.attr,
	&dev_attr_McASP_RBUF_3				.attr,
	&dev_attr_McASP_RBUF_4				.attr,
	&dev_attr_McASP_RBUF_5				.attr,
#endif
	&dev_attr_McASP_WFIFOCTL			.attr,
	&dev_attr_McASP_WFIFOSTS			.attr,
	&dev_attr_McASP_RFIFOCTL			.attr,
	&dev_attr_McASP_RFIFOSTS			.attr,
	NULL /* sentinel */
};

/* Declare attribute group (specifying in fact sysfs directory for your driver) */
static struct attribute_group mcasp_group = {
	.name ="regs",
	.attrs = mcasp_attrs,
};
/*--- End of data declaration --------------------------------------------------------------------*/

/*! This function creates sys_fs interface
 * and exposes all internal McASP peripheral registers to it.
 */
int mcasp_create_sys_iface(struct mcasp_device * mcasp_dev)
{
	int ret_val = 0;
	__PRINTK(YEL,"Creating 'sys' interface for '%s' controller.\n",
					mcasp_dev->plat_data->dev_name);

	if (mcasp_dev->dev == NULL)
	{
		__PRINTK(RED,"ERROR : Can't create 'sysfs' group. 'dev' is NULL.\n");
		return -1;
	}
	ret_val = sysfs_create_group(&mcasp_dev->dev->kobj, &mcasp_group);
	if (ret_val)
	{
		__PRINTK(RED,"'sysfs' group creation failed.\n");
		return ret_val;
	}
	else
	{
		__PRINTK(GRN,"'sysfs' group for '%s' controller successfully created.\n",
						mcasp_dev->plat_data->dev_name);
		__PRINTK(
				CYN,"Look files in folder:"
	"/sys/bus/platform/devices/%x.mcasp/regs\n",
				mcasp_dev->cfg_regs->start );
	}

	/// Store attribute group.
	mcasp_dev->attr_group = &mcasp_group;
	return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

void mcasp_destroy_sys_iface(struct mcasp_device * mcasp_dev)
{
	__PRINTK(RED,"Removing 'sysfs' interface.\n");
	if (mcasp_dev->dev!=NULL)
	{
		__PRINTK(RED,"ERROR : NULL passed as the parameter.\n");
		sysfs_remove_group(&mcasp_dev->dev->kobj, mcasp_dev->attr_group);
	}
}

void attr_finder	(
					struct mcasp_device * mcasp_dev,
					struct device_attribute * attr,
					void __iomem ** reg_addr,
					char * reg_name
					)
{
	if		(attr == (struct device_attribute *)&dev_attr_McASP_REV			.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_REV					; strcpy(reg_name,"REV"					);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_PWRIDLESYSCONFIG.attr)	{	*reg_addr = mcasp_dev->cfg_base + McASP_PWRIDLESYSCONFIG	; strcpy(reg_name,"PWRIDLESYSCONFIG"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_PFUNC		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_PFUNC				; strcpy(reg_name,"PFUNC"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_PDIR		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_PDIR				; strcpy(reg_name,"PDIR"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_PDOUT		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_PDOUT				; strcpy(reg_name,"PDOUT"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_PDIN		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_PDIN				; strcpy(reg_name,"PDIN"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_PDCLR		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_PDCLR				; strcpy(reg_name,"PDCLR"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_GBLCTL		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_GBLCTL				; strcpy(reg_name,"GBLCTL"		);	}
#if AMUTE_PRESENT
	else if (attr == (struct device_attribute *)&dev_attr_McASP_AMUTE		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_AMUTE				; strcpy(reg_name,"AMUTE"		);	}
#endif
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DLBCTL		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DLBCTL				; strcpy(reg_name,"DLBCTL"		);	}
#if (DIT_MODE)
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCTL		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCTL				; strcpy(reg_name,"DITCTL"		);	}
#endif
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RGBLCTL		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RGBLCTL				; strcpy(reg_name,"RGBLCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RMASK		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RMASK				; strcpy(reg_name,"RMASK"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RFMT		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RFMT				; strcpy(reg_name,"RFMT"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_AFSRCTL		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_AFSRCTL				; strcpy(reg_name,"AFSRCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_ACLKRCTL	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_ACLKRCTL			; strcpy(reg_name,"ACLKRCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_AHCLKRCTL	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_AHCLKRCTL			; strcpy(reg_name,"AHCLKRCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RTDM		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RTDM				; strcpy(reg_name,"RTDM"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RINTCTL		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RINTCTL				; strcpy(reg_name,"RINTCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RSTAT		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RSTAT				; strcpy(reg_name,"RSTAT"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RSLOT		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RSLOT				; strcpy(reg_name,"RSLOT"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RCLKCHK		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RCLKCHK				; strcpy(reg_name,"RCLKCHK"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_REVTCTL		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_REVTCTL				; strcpy(reg_name,"REVTCTL"	);	}
	
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XGBLCTL		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XGBLCTL				; strcpy(reg_name,"XGBLCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XMASK		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XMASK				; strcpy(reg_name,"XMASK"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XFMT		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XFMT				; strcpy(reg_name,"XFMT"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_AFSXCTL		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_AFSXCTL				; strcpy(reg_name,"AFSXCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_ACLKXCTL	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_ACLKXCTL			; strcpy(reg_name,"ACLKXCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_AHCLKXCTL	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_AHCLKXCTL			; strcpy(reg_name,"AHCLKXCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XTDM		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XTDM				; strcpy(reg_name,"XTDM"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XINTCTL		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XINTCTL				; strcpy(reg_name,"XINTCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XSTAT		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XSTAT				; strcpy(reg_name,"XSTAT"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XSLOT		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XSLOT				; strcpy(reg_name,"XSLOT"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XCLKCHK		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XCLKCHK				; strcpy(reg_name,"XCLKCHK"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XEVTCTL		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XEVTCTL				; strcpy(reg_name,"XEVTCTL"	);	}
#if (DIT_MODE)
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRA_0	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRA_0			; strcpy(reg_name,"DITCSRA_0"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRA_1	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRA_1			; strcpy(reg_name,"DITCSRA_1"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRA_2	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRA_2			; strcpy(reg_name,"DITCSRA_2"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRA_3	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRA_3			; strcpy(reg_name,"DITCSRA_3"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRA_4	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRA_4			; strcpy(reg_name,"DITCSRA_4"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRA_5	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRA_5			; strcpy(reg_name,"DITCSRA_5"	);	}

	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRB_0	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRB_0			; strcpy(reg_name,"DITCSRB_0"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRB_1	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRB_1			; strcpy(reg_name,"DITCSRB_1"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRB_2	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRB_2			; strcpy(reg_name,"DITCSRB_2"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRB_3	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRB_3			; strcpy(reg_name,"DITCSRB_3"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRB_4	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRB_4			; strcpy(reg_name,"DITCSRB_4"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITCSRB_5	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITCSRB_5			; strcpy(reg_name,"DITCSRB_5"	);	}

	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRA_0	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRA_0			; strcpy(reg_name,"DITUDRA_0"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRA_1	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRA_1			; strcpy(reg_name,"DITUDRA_1"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRA_2	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRA_2			; strcpy(reg_name,"DITUDRA_2"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRA_3	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRA_3			; strcpy(reg_name,"DITUDRA_3"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRA_4	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRA_4			; strcpy(reg_name,"DITUDRA_4"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRA_5	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRA_5			; strcpy(reg_name,"DITUDRA_5"	);	}

	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRB_0	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRB_0			; strcpy(reg_name,"DITUDRB_0"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRB_1	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRB_1			; strcpy(reg_name,"DITUDRB_1"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRB_2	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRB_2			; strcpy(reg_name,"DITUDRB_2"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRB_3	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRB_3			; strcpy(reg_name,"DITUDRB_3"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRB_4	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRB_4			; strcpy(reg_name,"DITUDRB_4"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_DITUDRB_5	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_DITUDRB_5			; strcpy(reg_name,"DITUDRB_5"	);	}
#endif

	else if (attr == (struct device_attribute *)&dev_attr_McASP_SRCTL_0		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_SRCTL_0				; strcpy(reg_name,"SRCTL_0"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_SRCTL_1		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_SRCTL_1				; strcpy(reg_name,"SRCTL_1"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_SRCTL_2		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_SRCTL_2				; strcpy(reg_name,"SRCTL_2"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_SRCTL_3		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_SRCTL_3				; strcpy(reg_name,"SRCTL_3"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_SRCTL_4		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_SRCTL_4				; strcpy(reg_name,"SRCTL_4"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_SRCTL_5		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_SRCTL_5				; strcpy(reg_name,"SRCTL_5"	);	}
#if (CFG_BUS)
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XBUF_0		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XBUF_0				; strcpy(reg_name,"XBUF_0"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XBUF_1		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XBUF_1				; strcpy(reg_name,"XBUF_1"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XBUF_2		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XBUF_2				; strcpy(reg_name,"XBUF_2"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XBUF_3		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XBUF_3				; strcpy(reg_name,"XBUF_3"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XBUF_4		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XBUF_4				; strcpy(reg_name,"XBUF_4"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_XBUF_5		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_XBUF_5				; strcpy(reg_name,"XBUF_5"		);	}

	else if (attr == (struct device_attribute *)&dev_attr_McASP_RBUF_0		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RBUF_0				; strcpy(reg_name,"RBUF_0"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RBUF_1		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RBUF_1				; strcpy(reg_name,"RBUF_1"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RBUF_2		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RBUF_2				; strcpy(reg_name,"RBUF_2"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RBUF_3		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RBUF_3				; strcpy(reg_name,"RBUF_3"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RBUF_4		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RBUF_4				; strcpy(reg_name,"RBUF_4"		);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RBUF_5		.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RBUF_5				; strcpy(reg_name,"RBUF_5"		);	}
#endif
	else if (attr == (struct device_attribute *)&dev_attr_McASP_WFIFOCTL	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_WFIFOCTL			; strcpy(reg_name,"WFIFOCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_WFIFOSTS	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_WFIFOSTS			; strcpy(reg_name,"WFIFOSTS"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RFIFOCTL	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RFIFOCTL			; strcpy(reg_name,"RFIFOCTL"	);	}
	else if (attr == (struct device_attribute *)&dev_attr_McASP_RFIFOSTS	.attr)		{	*reg_addr = mcasp_dev->cfg_base + McASP_RFIFOSTS			; strcpy(reg_name,"RFIFOSTS"	);	}

	else {
		strcpy(reg_name,"!!ERROR!!");
		reg_addr = NULL;
		__PRINTK(RED,"Can't find attribute file.n");
	}
	return;
}

/* Show/store methods */
static ssize_t mcasp_store	(
							struct device * dev,
							struct device_attribute * attr,
							const char * buf,
							size_t count
							)
{
	int						value;
	struct mcasp_device *	mcasp_dev;
	void __iomem *			reg_addr		= NULL;
	char					reg_name[16]	= { 0 };

	mcasp_dev = dev_get_drvdata(dev);
	if (!mcasp_dev){
		__PRINTK(RED,"Can't extract McASP module data.\r\n");
		return 0;
	}
	attr_finder(mcasp_dev, attr, &reg_addr, reg_name);
	value = simple_strtol(buf, NULL, 10);
#if McASP_SYS_DEBUG_SEVERE
	__PRINTK(CYN,"Writing 0x%x to the register %s[0x%p].\n", value, reg_name, reg_addr );
#endif
	if (reg_addr!=NULL) {
		writel(value, reg_addr);
#if McASP_SYS_DEBUG_SEVERE
		__PRINTK(CYN,"Success.\n");
#endif
		return count;
	} else {
		__PRINTK(RED,"Failure.\n");
		return count;
	}
}
/*--- End of function ----------------------------------------------------------------------------*/

static ssize_t mcasp_show	(
							struct device * dev,
							struct device_attribute * attr,
							char * buf
							)
{
	int						value			= 0;
	struct mcasp_device *	mcasp_dev;
	void __iomem *			reg_addr		= NULL;
	char					reg_name[16]	={0};

	mcasp_dev = dev_get_drvdata(dev);
	if (!mcasp_dev) {
		__PRINTK(RED,"Can't extract McASP module data.\r\n");
		return 0;
	}
	attr_finder(mcasp_dev, attr, &reg_addr, reg_name);
#if McASP_SYS_DEBUG_SEVERE
	__PRINTK(CYN,"Reading from the register %s[0x%p].\n", reg_name, reg_addr );
#endif
	if (reg_addr!=NULL) 
	{
		value = readl(reg_addr);
#if McASP_SYS_DEBUG_SEVERE
		__PRINTK(CYN,"%-10s (0x%p) register value : 0x%x.\n", reg_name, reg_addr, value );
#endif
		sprintf (buf,"%d", value);
		return strlen(buf);
	} else {
		__PRINTK(RED,"Failure.\n");
		return 0;
	}
}
/*--- End of function ----------------------------------------------------------------------------*/
