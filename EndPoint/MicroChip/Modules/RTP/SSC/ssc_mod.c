#include "ssc_mod.h"

/* Serialize access to ssc_list and user count */
static DEFINE_SPINLOCK(ssc_lock);
static LIST_HEAD(ssc_list);

/* Structures definitions */
/// This structure will be reside
/// inside #platform_device_id->driver_data (kernel_ulong_t)
/// and #of_device_id->data (void *).
static struct ssc_mod_plat_data ssc_dev_cfg = {
    .use_dma = 1,
    .has_fslen_ext = 1,
};
/*! In case of extracting platform data from driver itself. */
static const struct platform_device_id ssc_dev_types[] = {
    {
        .name = "lemz-ssc",
        .driver_data = (unsigned long) &ssc_dev_cfg,
    },
    {
        /* sentinel */
    }
};

#ifdef CONFIG_OF
static const struct of_device_id ssc_dev_tree_ids[] = {
    {
        .compatible = "lemz,lemz-ssc",
        .data = &ssc_dev_cfg,
    },
    {
        /* sentinel */
    }
};
MODULE_DEVICE_TABLE(of, ssc_dev_tree_ids);
#endif

#if (SSC_INIT_REG)
static const struct regmap_config ssc_regmap_config = {
	.reg_bits       = 32,                   ///< Number of bits in a register address, mandatory.

/*! The register address stride.
 * Valid register addresses are a multiple of this value.
 * If set to 0, a value of 1 will be used. */
	.reg_stride     = 4,

	.val_bits       = 32,                   ///< Number of bits in a register value, mandatory.
	.max_register   = SSC_WPSR_OFFSET,      ///< Optional, specifies the maximum valid register address.
};
#endif
/*--- End of structures definitions --------------------------------------------------------------*/

/*! DMA configuration routine
 *
 */
static int ssc_configure_dma(struct ssc_device * ssc_dev)
{
    int                             ret_val         = 0;
    /*! -# Define bus width of the DMA slave device, source or target buses. */

    __PRINTK(YEL, "Configuring DMA for SSC module.\n");
	memset(&ssc_dev->slave_config, 0, sizeof(ssc_dev->slave_config));

    /*! -# Provide the physical address where DMA slave data should be read(RX)/write(TX). */
    ssc_dev->slave_config.src_addr = (dma_addr_t)ssc_dev->regs->start + SSC_RHR_OFFSET; //RX
    ssc_dev->slave_config.dst_addr = (dma_addr_t)ssc_dev->regs->start + SSC_THR_OFFSET; //TX

    /*! -# Provide width in bytes of the source (RX)/(TX) register where DMA data shall be read/write. */
    ssc_dev->slave_config.src_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES; //RX
    ssc_dev->slave_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES; //TX

    /*! -# Set the maximum number of words that can be sent/receive in one burst to/from the device.
     * (note: words, as in units of the src_addr_width member, not bytes) */
    ssc_dev->slave_config.src_maxburst = 1;
    ssc_dev->slave_config.dst_maxburst = 1;
    /*! -# Flow Controller Settings. Only valid for slave channels.
     * Fill with 'true' if peripheral should be flow controller.
     * Direction will be selected at Runtime.*/
	ssc_dev->slave_config.device_fc = false;

    /* try to allocate an exclusive slave channel */
    ssc_dev->dma.tx_ch = dma_request_slave_channel_reason(
                                                    ssc_dev->dev,   //struct device
                                                    "tx"            //slave channel name
                                                    );
	if (IS_ERR(ssc_dev->dma.tx_ch)) {
        __PRINTK(RED, "DMA channel TX is not availible.\n");
		ret_val = PTR_ERR(ssc_dev->dma.tx_ch);
		ssc_dev->dma.tx_ch = NULL;
		goto error;
	}

	ssc_dev->dma.rx_ch = dma_request_slave_channel_reason(ssc_dev->dev, "rx");
	if (IS_ERR(ssc_dev->dma.rx_ch)) {
        __PRINTK(RED, "DMA channel RX is not availible.\n");
		ret_val = PTR_ERR(ssc_dev->dma.rx_ch);
		ssc_dev->dma.rx_ch = NULL;
		goto error;
	}

    ssc_dev->slave_config.direction = DMA_MEM_TO_DEV;
	if (dmaengine_slave_config(ssc_dev->dma.tx_ch, &ssc_dev->slave_config)) {
		__PRINTK(RED, "Failed to configure TX channel.\n");
		ret_val = -EINVAL;
		goto error;
	}

	ssc_dev->slave_config.direction = DMA_DEV_TO_MEM;
	if (dmaengine_slave_config(ssc_dev->dma.rx_ch, &ssc_dev->slave_config)) {
		__PRINTK(RED, "Failed to configure RX channel.\n");
		ret_val = -EINVAL;
		goto error;
	}

    sg_init_table(&ssc_dev->dma.sg[0], 1);
    sg_init_table(&ssc_dev->dma.sg[1], 1);

	__PRINTK(GRN,   "Using '%s' (TX) and '%s' (RX) for DMA transfers.\n",
                    dma_chan_name(ssc_dev->dma.tx_ch), dma_chan_name(ssc_dev->dma.rx_ch));
    return ret_val;

error:
    __PRINTK(RED, "Can't get DMA channel, continue without DMA support.\n");
	if (ssc_dev->dma.rx_ch) dma_release_channel(ssc_dev->dma.rx_ch);
	if (ssc_dev->dma.tx_ch) dma_release_channel(ssc_dev->dma.tx_ch);
	return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/


//struct ssc_device * ssc_request(unsigned int ssc_num)
//{
//    int ssc_valid = 0;
//    struct ssc_device *ssc;
//
//    spin_lock(&ssc_lock);
//    list_for_each_entry(ssc, &ssc_list, list) {
//        if (ssc->pdev->dev.of_node) {
//            if (of_alias_get_id(ssc->pdev->dev.of_node, "ssc")
//                == ssc_num) {
//                ssc->pdev->id = ssc_num;
//                ssc_valid = 1;
//                break;
//            }
//        } else if (ssc->pdev->id == ssc_num) {
//            ssc_valid = 1;
//            break;
//        }
//    }
//
//    if (!ssc_valid) {
//        spin_unlock(&ssc_lock);
//        pr_err("ssc: ssc%d platform device is missing\n", ssc_num);
//        return ERR_PTR(-ENODEV);
//    }
//
//    if (ssc->user) {
//        spin_unlock(&ssc_lock);
//        dev_dbg(&ssc->pdev->dev, "module busy\n");
//        return ERR_PTR(-EBUSY);
//    }
//    ssc->user++;
//    spin_unlock(&ssc_lock);
//
//    clk_prepare(ssc->gclk);
//
//    return ssc;
//}
//EXPORT_SYMBOL(ssc_request);
/*--- End of function ----------------------------------------------------------------------------*/


//void ssc_free(struct ssc_device *ssc)
//{
//    bool disable_clk = true;
//
//    spin_lock(&ssc_lock);
//    if (ssc->user)
//        ssc->user--;
//    else {
//        disable_clk = false;
//        dev_dbg(&ssc->pdev->dev, "device already free\n");
//    }
//    spin_unlock(&ssc_lock);
//
//    if (disable_clk)
//        clk_unprepare(ssc->gclk);
//}
//EXPORT_SYMBOL(ssc_free);
///*--- End of function ----------------------------------------------------------------------------*/


/*! Extract platform data (void *) from #platform_device structure passed by kernel.
 * @param pdev - Device represented by structure #platform_device. Provided by kernel.
 */
static inline const struct ssc_mod_plat_data * __init ssc_mod_get_driver_data(struct platform_device * pdev)
{
    const struct of_device_id * match;

    __PRINTK(YEL, "Extacting platform data for SSC module.\n");
    /// -# If associated device tree node #device_node is present.
    if (pdev->dev.of_node) {
        __PRINTK(YEL, "Extacting platform from 'platform_device' passed by kernel.\n");
        /// -# Look if a #device_node has a matching of_match structure.
        match = of_match_node(
                                ssc_dev_tree_ids, //Array of of device match structures to search in.
                                pdev->dev.of_node //The of device structure to match against.
                                );
        if (match == NULL) {
            __PRINTK(RED, "No platform data provided.\n");
            return NULL;
        }
        __PRINTK(GRN, "Platform data successfully extacted.\n");
        return match->data;
    }
    __PRINTK(YEL, "No platform data provided. Extracting data from module itself.\n");
    /// -# If no platfrom data provided, extract
    /// platform_device-> * platform_device_id-> kernel_ulong_t driver_data
    /// from driver itself. #of_device_id provided in #ssc_driver.
    return (struct ssc_mod_plat_data *)platform_get_device_id(pdev)->driver_data;
}
/*--- End of function ----------------------------------------------------------------------------*/

static int ssc_configure_clk (struct ssc_device * ssc_dev, struct platform_device * pdev)
{
        /// -#Extact audio clock
    ssc_dev->audiocoreclk_ptr   = devm_clk_get(&pdev->dev,"acclk");
    ssc_dev->audiopllclk_ptr    = devm_clk_get(&pdev->dev,"aclk");
    ssc_dev->audiopinclk_ptr    = devm_clk_get(&pdev->dev,"apclk");  
     
    /// -#Extact PCK
    //ssc_dev->prog0_clk          = devm_clk_get(&pdev->dev,"prog0");
    ssc_dev->prog1_clk          = devm_clk_get(&pdev->dev,"prog1");
    //ssc_dev->prog2_clk          = devm_clk_get(&pdev->dev,"prog2");
    
    /// -#Extract periph. clock
    ssc_dev->gclk               = devm_clk_get(&pdev->dev, "ssc0_clk");

    if      (IS_ERR(ssc_dev->gclk))             { __PRINTK(RED, "No peripheral clock provided.\n"       ); return -ENXIO; }
    else if (IS_ERR(ssc_dev->audiocoreclk_ptr)) { __PRINTK(RED, "No audio CORE clock provided.\n"       ); return -ENXIO; }
    else if (IS_ERR(ssc_dev->audiopllclk_ptr))  { __PRINTK(RED, "No audio PLL clock provided.\n"        ); return -ENXIO; }
    else if (IS_ERR(ssc_dev->audiopinclk_ptr))  { __PRINTK(RED, "No audio PIN clock provided.\n"        ); return -ENXIO; }
    //else if (IS_ERR(ssc_dev->prog0_clk))        { __PRINTK(RED, "No programmable clock 0 provided.\n"   ); return -ENXIO; }
    else if (IS_ERR(ssc_dev->prog1_clk))        { __PRINTK(RED, "No programmable clock 1 provided.\n"   ); return -ENXIO; }
    //else if (IS_ERR(ssc_dev->prog2_clk))        { __PRINTK(RED, "No programmable clock 2 provided.\n"   );  return -ENXIO; }
    else {
        __PRINTK(GRN, "Set up audio clock.\n");
        clk_set_rate(ssc_dev->audiocoreclk_ptr, 12288*54*1000);
        clk_set_rate(ssc_dev->audiopllclk_ptr,  12288*1*1000 );
        clk_set_rate(ssc_dev->audiopinclk_ptr,  12288*1*1000 );

        clk_prepare_enable(ssc_dev->audiopllclk_ptr);
        clk_prepare_enable(ssc_dev->audiopinclk_ptr);
        clk_prepare_enable(ssc_dev->audiocoreclk_ptr);
        
        __PRINTK(GRN, "Set up programmable clock.\n");
        //if (clk_set_parent(ssc_dev->prog0_clk, ssc_dev->audiopllclk_ptr)) { __PRINTK(RED, "ERROR : Can't set clock parent.\n"); }
        if (clk_set_parent(ssc_dev->prog1_clk, ssc_dev->audiopllclk_ptr)) { __PRINTK(RED, "ERROR : Can't set clock parent.\n"); }
        //if (clk_set_parent(ssc_dev->prog2_clk, ssc_dev->audiopllclk_ptr)) { __PRINTK(RED, "ERROR : Can't set clock parent.\n"); }
        
        //clk_set_rate(ssc_dev->prog0_clk,  2400*1000);
        clk_set_rate(ssc_dev->prog1_clk,  1024*1000);
        //clk_set_rate(ssc_dev->prog2_clk,  164000*1000);
        
        //clk_prepare_enable(ssc_dev->prog0_clk);
        clk_prepare_enable(ssc_dev->prog1_clk);
        //clk_prepare_enable(ssc_dev->prog2_clk);
        
        __PRINTK(GRN, "Enable peripheral clock.\n");
        clk_prepare_enable(ssc_dev->gclk);        
    }
    return 0;
}

/*! SSC module probe function
 * @param pdev  - Device represented by structure #platform_device.
 *                Will be passed by kernel after device tree parsing.
 */
static int ssc_probe(struct platform_device * pdev)
{
    struct resource *                       regs;
    struct ssc_device *                     ssc_dev;
    const struct ssc_mod_plat_data *        plat_data_ptr;
#if (SSC_INIT_REG)
    struct regmap *                         regmap;
#endif
    struct device_node *                    dev_node_ptr = pdev->dev.of_node;
    struct list_head *                      loop_cursor;

    __PRINTK(YEL, "Probing SSC peripheral.\n");

    if (!dev_node_ptr) __PRINTK(RED, "Associated device tree node isn't provided.\n");

    /// -# Allocate #ssc_device structure
#if (1)
    ssc_dev = devm_kzalloc(&pdev->dev, sizeof(struct ssc_device), GFP_KERNEL);
    //ssc_dev = kzalloc(sizeof(struct ssc_device), GFP_KERNEL);
    if (!ssc_dev) {
        __PRINTK(RED, "!!! OUT OF MEMORY !!!.\n");
        return -ENOMEM; /* Out of memory */
    }
#endif

    /// -# Store struct #platform_device and struct #device passed
    /// by kernel in #ssc_device structure.
    ssc_dev->pdev = pdev;
    ssc_dev->dev = &pdev->dev;

    /// -# Extact #ssc_mod_plat_data structure from #platform_device passed by kernel.
#if (1)
    plat_data_ptr = ssc_mod_get_driver_data(pdev);
    if (!plat_data_ptr){
        __PRINTK(RED, "No platform data for SSC module provided.\n");
        return -ENODEV; /* No such device */
    } else {
        __PRINTK(GRN, "Platform data for SSC module successfully extracted.\n");
    }
    /// -# Store #platform_device passed by kernel in #ssc_device structure.
    ssc_dev->pdata = (struct ssc_mod_plat_data *)plat_data_ptr;
#endif

    /// -# Define who should provide clock for SSC :
    /// 1) from SSC RK pin or 2) internally.
#if (1)
    __PRINTK(YEL, "Defining clock source : .\n");
    if (dev_node_ptr) {
        ssc_dev->clk_from_rk_pin = of_property_read_bool(dev_node_ptr, "atmel,clk-from-rk-pin");
        if (ssc_dev->clk_from_rk_pin)   { __PRINTK(MAG, " ! clock will be provided from SSC RK.\n");   }
        else                            { __PRINTK(MAG, " ! clock will be provided internally.\n");    }
    } else {
    __PRINTK(RED, "No associated device tree node provided.\n");
    }//end if (dev_node_ptr)
#endif

    /// -# Extract data about registers.
#if (1)
    regs = platform_get_resource    (
                                    pdev,           //struct platform_device
                                    IORESOURCE_MEM, //resource type
                                    0               //resource index
                                    );              //return struct resource
    __PRINTK(MAG, "SSC module has IO memory region : 0x%lx - 0x%lx.\n",
                (long unsigned int)regs->start,
                (long unsigned int)regs->end
                );
#endif

    /// -# Check, request region, and ioremap resource.
#if (1)
    ssc_dev->base = devm_ioremap_resource   (
                                        &pdev->dev, //generic device to handle the resource for (struct device)
                                        regs        //resource to be handled (struct resource)
                                        );
    if (IS_ERR(ssc_dev->base)){
        __PRINTK(RED, "Can't request region : 0x%lx - 0x%lx.\n",
                (long unsigned int)regs->start,
                (long unsigned int)regs->end
                );
        return PTR_ERR(ssc_dev->base);
    } else {
        __PRINTK(GRN, "IO region (0x%p ... ) for SSC module successfully remapped.\n", ssc_dev->base);
    }
#endif

    /// -# Initialise managed register map.
#if (SSC_INIT_REG)
    regmap = devm_regmap_init_mmio  (
                                    &pdev->dev,         //Device that will be interacted with.
                                    ssc_dev->base,      //Pointer to memory-mapped IO region.
                                    &ssc_regmap_config  //Configuration for register map
                                    );
    if (IS_ERR(regmap))
    {
        __PRINTK(RED, "Can't initialize IO region (0x%p ... )  for SSC module.\n", ssc_dev->base );
        return PTR_ERR(regmap);
    } else {
        __PRINTK(GRN, "IO region (0x%p ... ) for SSC module successfully initialized.\n", ssc_dev->base );
    }
#endif

    ssc_dev->mem_start = regs->start;

    /// -# Extact clock data.
    if (ssc_configure_clk (ssc_dev, pdev)) { __PRINTK(RED, "Can't configure clock resources"); return -ENXIO; }


    /// -# Extact IRQ # for the device.
#if (1)
    /// -# Disable all interrupts.
    ssc_dev->irq = platform_get_irq (
                                pdev,   //struct platform_device
                                0       //IRQ number index
                                );
    if (!ssc_dev->irq) {
        __PRINTK(RED, "No data about IRQ provided.\n");
        return -ENXIO;
    }
    __PRINTK(GRN, "IRQ data for SSC module successfully extracted. IRQ #%i.\n", ssc_dev->irq);
#endif

    /// -# Add structure #ssc_device to the #ssc_list.
#if (1)
    spin_lock(&ssc_lock);
        list_add_tail(&ssc_dev->list, &ssc_list);
        list_for_each(loop_cursor, &ssc_list)
        {
            ssc_dev->ctrlr_num++;
        }
        ssc_dev->ctrlr_num--; //exclude head of the list
    spin_unlock(&ssc_lock);
#endif

    /// -# Save structure #ssc_device as pdev->dev->driver_data for further use.
    /// Internally it is 'dev_set_drvdata(&pdev->dev, data)'.
    platform_set_drvdata(
                        pdev,   //struct platform_device
                        ssc_dev //Data to set (void *)
                        );

    ssc_configure_dma(ssc_dev);    
    ssc_create_sys_iface(ssc_dev);    
    ssc_create_dev_iface(ssc_dev);

    __PRINTK(GRN, "!!!SSC module successfully probed!!!\n");
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

static int ssc_remove(struct platform_device *pdev)
{
    struct ssc_device * ssc_dev = platform_get_drvdata(pdev);
    __PRINTK(RED, "Removing SSC module.\n");    
    ssc_destroy_dev_iface(ssc_dev);
    
    __PRINTK(RED, "Removing 'sysfs' interface.\n");
    sysfs_remove_group(&pdev->dev.kobj, ssc_dev->attr_group);

    spin_lock(&ssc_lock);
        list_del(&ssc_dev->list);
    spin_unlock(&ssc_lock);
    __PRINTK(RED, "Disable interrupts.\n");
	ssc_writel(ssc_dev->base, SSC_IDR_OFFSET, -1);

    __PRINTK(RED, "Disable DMA.\n");
    if (ssc_dev->dma.rx_ch) dma_release_channel(ssc_dev->dma.rx_ch);
	if (ssc_dev->dma.tx_ch) dma_release_channel(ssc_dev->dma.tx_ch);

    __PRINTK(RED, "Disable clock.\n");
    //clk_disable_unprepare(ssc_dev->prog0_clk        );
    clk_disable_unprepare(ssc_dev->prog1_clk        );
    //clk_disable_unprepare(ssc_dev->prog2_clk        );
    clk_disable_unprepare(ssc_dev->audiopllclk_ptr  );
    clk_disable_unprepare(ssc_dev->audiopinclk_ptr  );
    clk_disable_unprepare(ssc_dev->audiocoreclk_ptr );    
    clk_disable_unprepare(ssc_dev->gclk             );

    __PRINTK(RED, "!!!SSC module removed!!!\n");
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/


/*! Two mechanisms are provided :
 * 1) Find by name and #of_match_table that will be stored in kernel and passed back.
 * 2) Extract #id_table manually from module itself.
 */
static struct platform_driver ssc_driver = {
    .driver     = {
        .name       = "ssc",
        .of_match_table = of_match_ptr(ssc_dev_tree_ids),
    },
    .id_table   = ssc_dev_types,
    .probe      = ssc_probe, ///< As soon as #platform_driver_register succed this call will be made.
    .remove     = ssc_remove,
};
/*--- End of function ----------------------------------------------------------------------------*/

/*! @name Initialization facilities
 * @{ */
static int __init ssc_mod_init(void)
{
    __PRINTK(GRN, "Initializing TDM driver.\r\n");
    /*! Platform drivers make themselves known to the kernel. */
    return platform_driver_register(&ssc_driver);
}
/*--- End of function ----------------------------------------------------------------------------*/

static void __exit ssc_mod_exit(void)
{
    platform_driver_unregister(&ssc_driver);
    
    __PRINTK(RED, "Platform driver removed.\r\n"NORM);
}
/*! @} -------------------------------------------------------------------------------------------*/

module_init(ssc_mod_init);
module_exit(ssc_mod_exit);


MODULE_AUTHOR("microElk");
MODULE_DESCRIPTION("SSC/TDM driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:ssc");
