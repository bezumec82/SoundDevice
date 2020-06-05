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

#define PROG_CLK                DISABLE
#define McASP_INIT_REG          ENABLE
#define AUDIO_PIN_CLK           DISABLE
#define McASP_SYS_DEBUG_SEVERE  DISABLE
#define PERIPH_CLK              ENABLE

#define DEBUG_DEV               DISABLE

#define DEBUG_DMA               ENABLE
#define DEBUG_DMA_SEVERE        DISABLE
#define DMA_SIMPLE              ENABLE
#define DMA_CIRC_BUF            DISABLE
#define DEBUG_DMA_TAIL          ENABLE
#define DEBUG_DMA_CPY_USR       DISABLE

#define DIT_MODE                DISABLE
#define CFG_BUS                 DISABLE
#define AMUTE_PRESENT           0

#define BYTES_PER_MS                (16000/1000 * 8) /* two channels, 32 bits each, 16kHz */
#define DMA_BUF_SIZE                ( PAGE_SIZE * 8 )
#define DMA_BUF_PERIOD              ( BYTES_PER_MS * 4 )
#define DMA_BUF_QUANTUM             BYTES_PER_MS * 20
#define DMA_BUF_HEAD_OUTRUN         ( BYTES_PER_MS * 40 )
#define DMA_BUF_HEAD_MAX_OUTRUN     ( BYTES_PER_MS * 80 )

#define STAT_INTERVAL               ((DMA_BUF_PERIOD/BYTES_PER_MS)*64)


#include "mcasp_regs.h"
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

#else /* #ifdef DEBUG */
# define __PRINTK(color, text, ...)
# define DPRINTK(fmt, args...)
# define __PRINTK_RATELTD(color, text, ...)
#endif /* #ifdef DEBUG */

#if DMA_SIMPLE && DMA_CIRC_BUF
    #error "Can' implement both facilities."
#endif

#if !DMA_SIMPLE && !DMA_CIRC_BUF
    #error "Bad DMA configuration."
#endif

struct mcasp_dma {
    struct dma_chan *                   rx_ch;
    struct dma_chan *                   tx_ch;
                                        
    struct scatterlist                  sg_tx;
    struct scatterlist                  sg_rx;
                                        
    struct dma_slave_config             slave_config;
                                        
    void *                              dma_tx_buf;     ///< Scatter gather configuration
    size_t                              dma_tx_buf_len;

    /* In circ. buf. mode. */
    struct circ_buf                     tx_ring;
    dma_addr_t                          tx_addr;
    struct dma_async_tx_descriptor *    tx_desc;
    dma_addr_t                          tx_hdl;
    dma_cookie_t                        tx_cookie;

    atomic_t                            shutdown;
    struct tasklet_struct               clean_tasklet;

    int                                 prev_tx_tail;   ///< For clean buffer tasklet
    u32                                 tasklet_counter;

    u32                                 prev_stat_time;
    u32                                 prev_task_count;
    u32                                 necessary_task_count;
    u8                                  dma_started;

    spinlock_t                          lock;
    struct timer_list                   stat_timer;     ///< Timer to show statistics
};

/// Will be provided
struct mcasp_mod_plat_data {
        u8    ctrlr_num;            ///< Used to store controller number.
        char  dev_name    [32];     ///< Device name
};

/// Will be filled at the probe stage.
struct mcasp_device  {
    struct device_node *                dev_node;
    const struct mcasp_mod_plat_data *  plat_data;
    struct platform_device *            pdev;           ///< Store #platform_device passed by kernel.
    struct device *                     dev;
    
    struct resource *                   cfg_regs;
    struct resource *                   dat_regs;    
    void __iomem *                      cfg_base;       ///< Pointer to the start of remapped registers.
    void __iomem *                      dat_base;

     ///Provided clock.
    struct clk *                        fck;            ///< Peripheral clock
    ///Char device part
    dev_t                               dev_num;        ///< Device numbers (MAJOR:MINOR)
    struct mutex                        mutex;          ///< Mutual exclusion semaphore.
    struct cdev                         cdev;           ///< Char device structure.
    struct class *                      mcasp_class;    ///< To create '/dev' node.
    ///DMA
    struct mcasp_dma                    dma;
    ///IRQ
    int                                 rx_irq;         ///< Number of IRQ. Look DTS.
    int                                 tx_irq;
    ///Sys_fs interface
    struct attribute_group *            attr_group;
};

/* Functions prototypes */
int mcasp_create_sys_iface(struct mcasp_device * mcasp_dev);
void mcasp_destroy_sys_iface(struct mcasp_device * mcasp_dev);

int mcasp_create_dev_iface(struct mcasp_device * mcasp_dev);
int mcasp_destroy_dev_iface(struct mcasp_device * mcasp_dev);

int mcasp_configure_dma (struct mcasp_device * mcasp_dev);
void mcasp_disable_dma (struct mcasp_device * mcasp_dev);

ssize_t mcasp_write   (
                    struct file *       filp,
                    const char __user * usr_buf,
                    size_t              count,
                    loff_t *            f_pos
                    );

#define COPY_FROM_USR_ERR   __PRINTK(RED, "ERROR : Failed to get data from user.\n"); \
                            ret_val = -EFAULT; goto out;

/*--- End of functions prototypes ----------------------------------------------------------------*/

#endif /* __McASP_MOD_H__ */
