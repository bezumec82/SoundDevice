/*! @file driver.h
 * @brief Main header of the module.
 */

#ifndef __DRIVER_H
#define __DRIVER_H

#define RAW_IRQ_HANDLERS 0
//#define DEBUG 1
//#define MEMORY_DEBUG

#define DSP_IRQ_MAX_COUNT 1000

/*! @cond DO_NOT_SHOW ----------------------------------------------------------------------------*/
#include <linux/version.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/dma.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/poll.h>
#include <asm/unistd.h>
#include <asm/byteorder.h>
#include <asm/atomic.h>
#include <linux/ip.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/proc_fs.h>
#include <asm/byteorder.h>
#if defined(HAL_X86)
#include <linux/devfs_fs_kernel.h>
#include <asm/elan.h>
#include "memdebug.h"
#elif defined(HAL_PPC)
#include <linux/devfs_fs_kernel.h>
#include <asm/system.h>
//#include <asm/immap_8260.h>
#include "memdebug.h"
#endif
/*! @endcond -------------------------------------------------------------------------------------*/

#include "hal.h"
#include "cb.h"
#include "channel.h"
#include "codec.h"
#include "vcodec.h"
#include "vmux.h"

#include "voice-ioctl.h"
#include "clk-common.h"


#define HAL_MAJOR                   237
#define HAL_DEVNAME                 "hal"
#define HAL_DESCR                   "SoundDevice 1.0"
/// Used in global variable #init.
#define MAX_ENABLE_STRS             64

//#define SYS_CLOCK_ABSENT            65535000 /* NOT USED */

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
                    printk(color"%-20.20s %-20.20s #%-5i: "text""NORM, \
                    __FILENAME__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); 
                       
# define DPRINTK(fmt, args...) printk("%s: " fmt, __FUNCTION__ , ## args)

# define __PRINTK_RATELTD(color, text, ...) \
                    printk_ratelimited(color"%-20.20s %-20.20s #%-5i: "text""NORM, \
                    __FILENAME__, __FUNCTION__, __LINE__ , ##__VA_ARGS__); 
           
#else /* #ifdef DEBUG */

# define __PRINTK(color, text, ...)
# define DPRINTK(fmt, args...)
# define __PRINTK_RATELTD(color, text, ...)
#endif /* #ifdef DEBUG */

#define PRINTK(level, fmt, args...) printk(level HAL_DEVNAME": " fmt, ## args)
/*! @} -------------------------------------------------------------------------------------------*/

/*! @cond DO_NOT_SHOW ----------------------------------------------------------------------------*/
/* NOT USED
#define CLEANUP_LIST(list, type, member, action, list_entry_name)               \
{                                                                               \
    for (;;) {                                                                  \
        struct list_head *tmp = ((struct list_head *)(list))->next;             \
        type *list_entry_name;                                                  \
        if (tmp == list) break;                                                 \
        list_del(tmp);                                                          \
        list_entry_name = list_entry(tmp, type, member);                        \
        action;                                                                 \
        free(list_entry_name);                                                  \
    }                                                                           \
}
*/
/*! @endcond -------------------------------------------------------------------------------------*/

/*!
 * @{ --------------------------------------------------------------------------------------------*/
//typedef int error_t; /* NOT USED */
/*! @} -------------------------------------------------------------------------------------------*/

/*! @name Not used.
 * @{ */
void module_inc_use_count(void);
void module_dec_use_count(void);
/*! @} -------------------------------------------------------------------------------------------*/


/*! @cond DO_NOT_SHOW ----------------------------------------------------------------------------*/
extern char * init[];
extern WAIT_QUEUE(hal_poll_wq);
/*! @endcond -------------------------------------------------------------------------------------*/

/*!
 * @{ @name Not used. */
struct hal_context {
    struct {
        struct timer_list wd_timer;
        int prev_reset;
    } mux_ctx;
};
/*! @} -------------------------------------------------------------------------------------------*/

#endif /* __DRIVER_H */
