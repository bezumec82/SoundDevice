#ifndef __SSC_MOD_H__
#define __SSC_MOD_H__

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
/*! @endcond -------------------------------------------------------------------------------------*/

#include "../hal/driver.h"
#include "ssc_regs.h"

#define ENABLE                  1
#define DISABLE                 0
#define SSC_INIT_REG            ENABLE
#define SSC_MOD_DEBUG           ENABLE
#define SSC_SYS_DEBUG_SEVERE    DISABLE

struct ssc_mod_plat_data {
    int         use_dma;
    int         has_fslen_ext;
};

struct ssc_dma {
	struct dma_chan *       rx_ch;
	struct dma_chan *       tx_ch;
    struct scatterlist      sg[2];
};

struct ssc_device  {
    struct device *                     dev;
    struct list_head                    list;

    void __iomem *                      base;       ///< Pointer to the start of remapped registers.
    struct platform_device *            pdev;       ///< Store #platform_device passed by kernel.
    struct ssc_mod_plat_data *          pdata;      ///< Store #platform_device passed by kernel.
    
    struct clk *                        gclk;       ///< Describes provided clock.
    
    struct clk *                        audiocoreclk_ptr;
    struct clk *                        audiopllclk_ptr;
    struct clk *                        audiopinclk_ptr;    ///< Provides clock to the DSP/CDC
    
    struct clk *                        prog0_clk;  ///< Programmable clock provides clock to RK pin
    struct clk *                        prog1_clk;
    struct clk *                        prog2_clk;
    ///Char device part
    dev_t                               dev_num;        ///<Device numbers (MAJOR:MINOR)
    struct mutex                        mutex;          ///<Mutual exclusion semaphore.
    struct cdev                         cdev;	        ///<Char device structure.
    char                                dev_name[16];   ///<Device name
    struct class *                      ssc_class;      ///<To create '/dev' node.
    ///DMA
    struct ssc_dma                      dma;            ///< DMA setup
    struct dma_slave_config             slave_config;
    dma_addr_t                          mem_start;      ///< Start of the IO regs memory.
    size_t                              dma_buf_len;
    void *                              dma_buf;
    
    int                                 irq;                ///< Number of IRQ. Look DTS.
    bool                                clk_from_rk_pin;    ///< Defines where to take clock from.
    
    ///Sys_fs interface
    struct attribute_group *            attr_group;
    unsigned int                        ctrlr_num;  ///< Used to store controller number.
};

/* Functions prototypes */
//struct ssc_device * __must_check    ssc_request             (unsigned int        ssc_num);
void                                ssc_free                (struct ssc_device * ssc);
int                                 ssc_create_sys_iface    (struct ssc_device * ssc_dev);
int                                 ssc_create_dev_iface    (struct ssc_device * ssc_dev);
int                                 ssc_destroy_dev_iface   (struct ssc_device * ssc_dev);
/*--- End of functions prototypes ----------------------------------------------------------------*/

#endif /* __SSC_MOD_H__ */
