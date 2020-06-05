#ifndef __I2S_MOD_H__
#define __I2S_MOD_H__

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
/*! @endcond -------------------------------------------------------------------------------------*/

#include "../hal/driver.h"
#include "i2s_regs.h"

#define ENABLE                  1
#define DISABLE                 0

#define PROG_CLK                DISABLE
#define I2S_INIT_REG            ENABLE
#define AUDIO_PIN_CLK           DISABLE
#define I2S_SYS_DEBUG_SEVERE    DISABLE
#define PERIPH_CLK              ENABLE

#define DEBUG_DEV               DISABLE

#define DEBUG_DMA               ENABLE
#define DEBUG_DMA_SEVERE        DISABLE
#define DMA_SIMPLE              DISABLE
#define DMA_CIRC_BUF            ENABLE
#define DEBUG_DMA_TAIL          ENABLE
#define DEBUG_DMA_CPY_USR       DISABLE



#define BYTES_PER_MS                (16000/1000 * 8) /* two channels, 32 bits each, 16kHz */
#define DMA_BUF_SIZE                ( PAGE_SIZE * 8 )
#define DMA_BUF_PERIOD              ( BYTES_PER_MS * 4 )
#define DMA_BUF_QUANTUM             BYTES_PER_MS * 20
#define DMA_BUF_HEAD_OUTRUN         ( BYTES_PER_MS * 40 )
#define DMA_BUF_HEAD_MAX_OUTRUN     ( BYTES_PER_MS * 80 )

#define STAT_INTERVAL               ((DMA_BUF_PERIOD/BYTES_PER_MS)*64)

#if DMA_SIMPLE && DMA_CIRC_BUF
    #error "Can' implement both facilities."
#endif

#if !DMA_SIMPLE && !DMA_CIRC_BUF
    #error "Bad DMA configuration."
#endif

struct i2s_dma {
	struct dma_chan *       rx_ch;
	struct dma_chan *       tx_ch;
    
    struct scatterlist      sg_tx;
    struct scatterlist      sg_rx;
    
    struct dma_slave_config slave_config;
    
    void *                  dma_tx_buf;                 ///< Scatter gather configuration
    size_t                  dma_tx_buf_len;
    
    /* In circ. buf. mode. */
    struct circ_buf                     tx_ring;
    dma_addr_t                          tx_addr;
    struct dma_async_tx_descriptor *    tx_desc;
    dma_addr_t                          tx_hdl;
    dma_cookie_t                        tx_cookie;
    
    atomic_t		                    shutdown;
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
struct i2s_mod_plat_data {
        u8    ctrlr_num;            ///< Used to store controller number.
        char  dev_name    [16];     ///< Device name
        char  gclk_name   [16];     ///< Device name
        char  pclk_name   [16];     ///< Device name
};

/// Will be filled at the probe stage.
struct i2s_device  {
    struct device_node *                dev_node;
    const struct i2s_mod_plat_data *    plat_data;
    struct platform_device *            pdev;           ///< Store #platform_device passed by kernel.
    struct device *                     dev;
    struct resource *                   regs;
    void __iomem *                      base;           ///< Pointer to the start of remapped registers.
        
     ///Provided clock.
    struct clk *                        pclk;           ///< Peripheral clock   
    struct clk *                        gclk;           ///< Clock from generic clock controller       
    struct clk *                        AUDIOCORECLK;
    struct clk *                        AUDIOPLLCLK;
    struct clk *                        AUDIOPINCLK;    ///< Provides clock to the DSP/CDC
    ///Programmable clock pins
#if (PROG_CLK) 
    struct clk *                        prog0_clk;      
    struct clk *                        prog1_clk;
    struct clk *                        prog2_clk;
#endif
    ///Char device part
    dev_t                               dev_num;        ///< Device numbers (MAJOR:MINOR)
    struct mutex                        mutex;          ///< Mutual exclusion semaphore.
    struct cdev                         cdev;	        ///< Char device structure.

    struct class *                      i2s_class;      ///< To create '/dev' node.
    ///DMA
    struct i2s_dma                      dma;
    
    ///IRQ
    int                                 irq;            ///< Number of IRQ. Look DTS.        
    ///Sys_fs interface
    struct attribute_group *            attr_group; 
};

/* Functions prototypes */
int i2s_create_sys_iface(struct i2s_device * );
void i2s_destroy_sys_iface(struct i2s_device * i2s_dev);

int i2s_create_dev_iface(struct i2s_device * i2s_dev);
int i2s_destroy_dev_iface(struct i2s_device * i2s_dev);

int i2s_configure_dma (struct i2s_device * i2s_dev);
void i2s_disable_dma (struct i2s_device * i2s_dev);

ssize_t i2s_write   (
                    struct file *       filp, 
                    const char __user * usr_buf, 
                    size_t              count,
                    loff_t *            f_pos
                    );
                    
#define COPY_FROM_USR_ERR   __PRINTK(RED, "ERROR : Failed to get data from user.\n"); \
                            ret_val = -EFAULT; goto out;

/*--- End of functions prototypes ----------------------------------------------------------------*/

#endif /* __I2S_MOD_H__ */
