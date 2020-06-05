#ifndef __McASP_MOD_H__
#define __McASP_MOD_H__

/*! @cond DO_NOT_SHOW ----------------------------------------------------------------------------*/
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/regmap.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/types.h>
#include <linux/sysfs.h>
#include <linux/cdev.h>
#include <linux/mfd/syscon.h>
#include <linux/circ_buf.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/pm_runtime.h>
/*! @endcond -------------------------------------------------------------------------------------*/

#define ENABLE                  1
#define DISABLE                 0

/*! @addtogroup Console_output
 * @{ */
#define NORM                "\x1B[0m"   ///normal
#define RED                 "\x1B[31m"  ///red
#define GRN                 "\x1B[32m"  ///green
#define YEL                 "\x1B[33m"  ///yellow
#define BLU                 "\x1B[34m"  ///blue
#define MAG                 "\x1B[35m"  ///magenta
#define CYN                 "\x1B[36m"  ///cyan
#define WHT                 "\x1B[37m"  ///white
#define BOLD                "\e[1m"     ///bold symbols
#define ITAL                "\e[3m"     ///italic text
#define BLINK               "\e[5m"     ///blink


/* Strip down path from filename */
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#ifdef DEBUG

# define __PRINTK(color, text, ...) \
                    printk(color"%-15.15s %-15.15s #%-5i: "text""NORM, \
                    __FILENAME__, __FUNCTION__, __LINE__ , ##__VA_ARGS__);

# define DPRINTK(fmt, args...) printk("%s: " fmt, __FUNCTION__ , ## args)

# define __PRINTK_RATELTD(color, text, ...) \
                    printk_ratelimited(color"%-20.20s %-20.20s #%-5i: "text""NORM, \
                    __FILENAME__, __FUNCTION__, __LINE__ , ##__VA_ARGS__);
#define __PRINTERR(text, ...)   __PRINTK(RED, "ERROR : "text""NORM, ##__VA_ARGS__);

#else /* #ifdef DEBUG */
# define __PRINTK(color, text, ...)
# define DPRINTK(fmt, args...)
# define __PRINTK_RATELTD(color, text, ...)
#endif /* #ifdef DEBUG */


/// Will be filled at the probe stage.
struct mcasp_device  {
    struct device_node *     dev_node;
    struct platform_device * pdev;           ///< Store #platform_device passed by kernel.
    struct device *          dev;
    struct clk *             fck;            ///< Peripheral clock
    char                     clk_name[32];
};

/* Functions prototypes */

/*--- End of functions prototypes ----------------------------------------------------------------*/

#endif /* __McASP_MOD_H__ */
