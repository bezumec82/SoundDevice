#include "i2s_mod.h"

#define PROG0_FREQ  ( 12288 * 1000 )
#define PROG1_FREQ  ( 12288 * 1000 )
#define PROG2_FREQ  ( 12288 * 1000 )

#define AUDIOCORECLK_FREQ   ( 12288 * 1000 * 54 )
#define AUDIOPLLCLK_FREQ    ( 12288 * 1000 * 6 )
#define AUDIOPINCLK_FREQ    ( 12288 * 1000 * 1 )

#define GCLK_FREQ           ( 12288 * 1000 * 2 )

static const struct i2s_mod_plat_data i2s0_data = {
        .ctrlr_num  = 0,           
        .dev_name   = "i2s0",    
        .gclk_name  = "i2s0_gclk",
        .pclk_name  = "i2s0_pclk",  
};

static const struct i2s_mod_plat_data i2s1_data = {
        .ctrlr_num  = 1,           
        .dev_name   = "i2s1",
        .gclk_name  = "i2s1_gclk",  
        .pclk_name  = "i2s1_pclk",  
};

static const struct of_device_id i2s_dev_tree_ids[] = {
    {
        .compatible = "lemz,lemz-i2s0",
        .data = &i2s0_data,
    },
    {
        .compatible = "lemz,lemz-i2s1",
        .data = &i2s1_data,
    },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, i2s_dev_tree_ids);

static const struct platform_device_id i2s_dev_types[] = {
    {
        .name = "lemz-i2s0",
        .driver_data = (unsigned long) &i2s0_data,
    },
    {
        .name = "lemz-i2s1",
        .driver_data = (unsigned long) &i2s1_data,
    },
    { /* sentinel */ }
};

#if (I2S_INIT_REG)
static const struct regmap_config i2s_regmap_config = {
	.reg_bits       = 32,                   ///< Number of bits in a register address, mandatory.

/*! The register address stride.
 * Valid register addresses are a multiple of this value.
 * If set to 0, a value of 1 will be used. */
	.reg_stride     = 4,

	.val_bits       = 32,           ///< Number of bits in a register value, mandatory.
	.max_register   = I2SC_THR_OFF, ///< Optional, specifies the maximum valid register address.
};
#endif
/*--- End of data declaration --------------------------------------------------------------------*/

/*! Extract platform data (void *) from #platform_device structure passed by kernel.
 * @param pdev - Device represented by structure #platform_device. Provided by kernel.
 */
static inline const struct i2s_mod_plat_data * __init i2s_mod_get_driver_data(struct platform_device * pdev)
{
    const struct of_device_id * match;

    __PRINTK(YEL, "Extacting platform data for I2S module.\n");
    /// -# If associated device tree node #device_node is present.
    if (pdev->dev.of_node) {
        __PRINTK(YEL, "Extacting platform from 'platform_device' passed by kernel.\n");
        /// -# Look if a #device_node has a matching of_match structure.
        match = of_match_node(
                                i2s_dev_tree_ids, //Array of of device match structures to search in.
                                pdev->dev.of_node //The of device structure to match against.
                                );
        if (match == NULL) {
            __PRINTK(RED, "No platform data provided.\n");
            return NULL;
        }
        __PRINTK(GRN,   "Platform data for '%s' controller successfully extacted.\n", 
                        ((struct i2s_mod_plat_data *)match->data)->dev_name);
        return (struct i2s_mod_plat_data *)match->data;
    }
    __PRINTK(RED, "No platform data provided. Extracting data from module itself.\n");
    /// -# If no platfrom data provided, extract
    /// platform_device-> * platform_device_id-> kernel_ulong_t driver_data
    /// from driver itself. #of_device_id provided in #ssc_driver.
    return (struct i2s_mod_plat_data *)platform_get_device_id(pdev)->driver_data;
}
/*--- End of function ----------------------------------------------------------------------------*/

static int switch_I2SCLKSEL(struct i2s_device * i2s_dev)
{
	#define SFR_I2SCLKSEL_OFF           0x90U
	struct regmap * sfr;   
	sfr = syscon_regmap_lookup_by_compatible("atmel,sama5d2-sfr");
	if (IS_ERR(sfr)) 
    {
		__PRINTK(RED, "ERROR : Failed to get SFR syscon\n");
		return PTR_ERR(sfr);
	}
     __PRINTK(CYN, "Switching I2SCLKSEL to GCLK. Reg. Ctrlr : %d.\n", i2s_dev->plat_data->ctrlr_num);
    if          (i2s_dev->plat_data->ctrlr_num == 0)
    {
        return regmap_update_bits(sfr, SFR_I2SCLKSEL_OFF, 0b01, 0b01);
    }
    else if     (i2s_dev->plat_data->ctrlr_num == 1)
        return regmap_update_bits(sfr, SFR_I2SCLKSEL_OFF, 0b10, 0b10);
    else
    {
        __PRINTK(RED, "ERROR : Controller number : %d.\n", i2s_dev->plat_data->ctrlr_num);
        return -1;
    }
}

static int i2s_configure_clk (struct i2s_device * i2s_dev, struct platform_device * pdev)
{    
    /// -#Extact audio clock
    i2s_dev->AUDIOCORECLK       = devm_clk_get(&pdev->dev,"acclk");
    i2s_dev->AUDIOPLLCLK        = devm_clk_get(&pdev->dev,"aclk");
    
#if (AUDIO_PIN_CLK)
    i2s_dev->AUDIOPINCLK        = devm_clk_get(&pdev->dev,"apclk");
#endif

#if (PROG_CLK) 
    /// -#Extact PCK
    i2s_dev->prog0_clk          = devm_clk_get(&pdev->dev,"prog0");
    i2s_dev->prog1_clk          = devm_clk_get(&pdev->dev,"prog1");
    i2s_dev->prog2_clk          = devm_clk_get(&pdev->dev,"prog2");
#endif

    /// -#Extract periph. clock
    i2s_dev->pclk               = devm_clk_get(&pdev->dev, i2s_dev->plat_data->pclk_name);
    i2s_dev->gclk               = devm_clk_get(&pdev->dev, i2s_dev->plat_data->gclk_name);

    if      (IS_ERR(i2s_dev->gclk))             { __PRINTK(RED, "No peripheral clock provided.\n"       ); return -ENXIO; }
    else if (IS_ERR(i2s_dev->pclk))             { __PRINTK(RED, "No generic clock provided.\n"          ); return -ENXIO; }
    else if (IS_ERR(i2s_dev->AUDIOCORECLK))     { __PRINTK(RED, "No audio CORE clock provided.\n"       ); return -ENXIO; }
    else if (IS_ERR(i2s_dev->AUDIOPLLCLK))      { __PRINTK(RED, "No audio PLL clock provided.\n"        ); return -ENXIO; }
#if (AUDIO_PIN_CLK)
    else if (IS_ERR(i2s_dev->AUDIOPINCLK))      { __PRINTK(RED, "No audio PIN clock provided.\n"        ); return -ENXIO; }
#endif
#if (PROG_CLK) 
    else if (IS_ERR(i2s_dev->prog0_clk))        { __PRINTK(RED, "No programmable clock 0 provided.\n"   ); return -ENXIO; }
    else if (IS_ERR(i2s_dev->prog1_clk))        { __PRINTK(RED, "No programmable clock 1 provided.\n"   ); return -ENXIO; }
    else if (IS_ERR(i2s_dev->prog2_clk))        { __PRINTK(RED, "No programmable clock 2 provided.\n"   ); return -ENXIO; }
#endif
    else 
    {
        __PRINTK(GRN, "Set up audio clock.\n");
        clk_set_rate(i2s_dev->AUDIOCORECLK, AUDIOCORECLK_FREQ   );
        clk_set_rate(i2s_dev->AUDIOPLLCLK,  AUDIOPLLCLK_FREQ    );
        
#if (AUDIO_PIN_CLK)
        clk_set_rate(i2s_dev->AUDIOPINCLK,  AUDIOPINCLK_FREQ    );
#endif

        clk_prepare_enable(i2s_dev->AUDIOCORECLK    );
        clk_prepare_enable(i2s_dev->AUDIOPLLCLK     );
        
#if (AUDIO_PIN_CLK)
        clk_prepare_enable(i2s_dev->AUDIOPINCLK     );
#endif
    
#if (PROG_CLK) 
        __PRINTK(GRN, "Set up programmable clock.\n");
        if (clk_set_parent(i2s_dev->prog0_clk, i2s_dev->audiopllclk_ptr)) { __PRINTK(RED, "ERROR : Can't set 'prog0' clock parent.\n"); }
        if (clk_set_parent(i2s_dev->prog1_clk, i2s_dev->audiopllclk_ptr)) { __PRINTK(RED, "ERROR : Can't set 'prog1' clock parent.\n"); }
        if (clk_set_parent(i2s_dev->prog2_clk, i2s_dev->audiopllclk_ptr)) { __PRINTK(RED, "ERROR : Can't set 'prog2' clock parent.\n"); }
        clk_set_rate(i2s_dev->prog0_clk,  PROG0_FREQ);
        clk_set_rate(i2s_dev->prog1_clk,  PROG1_FREQ);
        clk_set_rate(i2s_dev->prog2_clk,  PROG2_FREQ);
        clk_prepare_enable(i2s_dev->prog0_clk);
        clk_prepare_enable(i2s_dev->prog1_clk);
        clk_prepare_enable(i2s_dev->prog2_clk);
#endif

        __PRINTK(YEL, "Enable '%s' clock.\n", i2s_dev->plat_data->gclk_name);
        clk_set_rate(i2s_dev->gclk, GCLK_FREQ);
        clk_prepare_enable(i2s_dev->gclk);
        
#if (PERIPH_CLK)
        clk_prepare_enable(i2s_dev->pclk);
#endif
    }
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/
static void i2s_disable_clk (struct i2s_device * i2s_dev)
{
    __PRINTK(RED, "Disabling clock for controller '%s'.\n", i2s_dev->plat_data->dev_name);
#if (PROG_CLK)
    clk_disable_unprepare(i2s_dev->prog0_clk    );
    clk_disable_unprepare(i2s_dev->prog1_clk    );
    clk_disable_unprepare(i2s_dev->prog2_clk    );
#endif
#if (AUDIO_PIN_CLK)
    clk_disable_unprepare(i2s_dev->AUDIOPINCLK  );
#endif
    clk_disable_unprepare(i2s_dev->AUDIOPLLCLK  );
    clk_disable_unprepare(i2s_dev->AUDIOCORECLK );    
    clk_disable_unprepare(i2s_dev->gclk         );
#if (PERIPH_CLK)
    clk_disable_unprepare(i2s_dev->pclk         );
#endif
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! I2S module probe function
 * @param pdev  - Device represented by structure #platform_device.
 *                Will be passed by kernel after device tree parsing.
 */
static int i2s_probe(struct platform_device * pdev)
{
    struct i2s_device *     i2s_dev;
#if (I2S_INIT_REG)
    struct regmap *         regmap;
#endif
	
    if(!pdev)
    {
        __PRINTK(RED, "ERROR : Platform device not provided.\n" );
        return -1;
    }    
    __PRINTK(YEL, "Probing '%s' controller.\n", pdev->name);
    printk("!!! HZ = %i !!!", HZ);
    /// -# Allocate #i2s_device structure
#if (1)
    i2s_dev = devm_kzalloc(&pdev->dev, sizeof(struct i2s_device), GFP_KERNEL);
    //ssc_dev = kzalloc(sizeof(struct ssc_device), GFP_KERNEL);
    if (!i2s_dev) {
        __PRINTK(RED, "!!! OUT OF MEMORY !!!.\n");
        return -ENOMEM; /* Out of memory */
    }
#endif 
    i2s_dev->plat_data = i2s_mod_get_driver_data(pdev);
    if (!i2s_dev->plat_data) 
    {
        __PRINTK(RED, "ERROR : Can't extract platform data.\n" );
        return -1;
    }
    /// -# Store struct #platform_device and struct #device passed
    /// by kernel in #i2s_device structure.
    i2s_dev->pdev = pdev;
    i2s_dev->dev = &pdev->dev;
    i2s_dev->dev_node = pdev->dev.of_node;
    /// -# Extract data about registers.
#if (1)
    i2s_dev->regs = platform_get_resource   (               //return struct resource
                                            pdev,           //struct platform_device
                                            IORESOURCE_MEM, //resource type
                                            0               //resource index
                                            );
    if (!i2s_dev->regs)
    {
        __PRINTK(RED, "Can't extract register data from device tree.\n");
        return -1;
    }
    __PRINTK(GRN,   "'%s' controller has IO memory region : 0x%lx - 0x%lx.\n",
                    i2s_dev->plat_data->dev_name,
                    (long unsigned int)i2s_dev->regs->start,
                    (long unsigned int)i2s_dev->regs->end
                    );
    /// -# Check, request region, and ioremap resource.
    i2s_dev->base = devm_ioremap_resource   (
                                            &pdev->dev,     //generic device to handle the resource for (struct device)
                                            i2s_dev->regs   //resource to be handled (struct resource)
                                            );
    if (IS_ERR(i2s_dev->base)){
        __PRINTK(RED,   "Can't request region : 0x%lx - 0x%lx for '%s' controller.\n",
                        (long unsigned int)i2s_dev->regs->start,
                        (long unsigned int)i2s_dev->regs->end,
                        i2s_dev->plat_data->dev_name
                        );
        return PTR_ERR(i2s_dev->base);
    } else {
        __PRINTK(GRN,   "IO region (0x%p ... ) for '%s' controller successfully remapped.\n", 
                        i2s_dev->base, i2s_dev->plat_data->dev_name);
    }
    /// -# Initialise managed register map.
#if (I2S_INIT_REG)
    regmap = devm_regmap_init_mmio  (
                                    &pdev->dev,         //Device that will be interacted with.
                                    i2s_dev->base,      //Pointer to memory-mapped IO region.
                                    &i2s_regmap_config  //Configuration for register map
                                    );
    if (IS_ERR(regmap))
    {
        __PRINTK(RED, "Can't initialize IO region (0x%p ... )  for I2S module.\n", i2s_dev->base );
        return PTR_ERR(regmap);
    } else {
        __PRINTK(GRN,   "IO region (0x%p ... ) for '%s' controller successfully initialized.\n", 
                        i2s_dev->base,  i2s_dev->plat_data->dev_name);
    }
#endif
#endif

    switch_I2SCLKSEL(i2s_dev);
    /// -# Extact clock data.
    if (i2s_configure_clk (i2s_dev, pdev)) 
    { 
        __PRINTK(RED,   "Can't configure clock resources for controller '%s'.\n", 
                        i2s_dev->plat_data->dev_name);
        return -ENXIO; 
    }
    /// -# Extact IRQ # for the device.
#if (1)
    /// -# Disable all interrupts.
    i2s_dev->irq = platform_get_irq (
                                    pdev,   //struct platform_device
                                    0       //IRQ number index
                                    );
    if (!i2s_dev->irq) 
    {
        __PRINTK(RED,   "No data about IRQ provided for controller '%s'.\n", 
                        i2s_dev->plat_data->dev_name);
        return -ENXIO;
    }
    __PRINTK(GRN, "IRQ data for I2S module successfully extracted. IRQ #%i.\n", i2s_dev->irq);
#endif

    /// -# Save structure #i2s_device as pdev->dev->driver_data for further use.
    /// Internally it is 'dev_set_drvdata(&pdev->dev, data)'.
    platform_set_drvdata(
                        pdev,   //struct platform_device
                        i2s_dev //Data to set (void *)
                        );
    if (i2s_configure_dma(i2s_dev)) 
    { 
        __PRINTK(RED,   "Can't configure DMA for controller '%s'.\n", 
                        i2s_dev->plat_data->dev_name);
        return -ENXIO; 
    }
    i2s_create_sys_iface(i2s_dev);
    i2s_create_dev_iface(i2s_dev);
    __PRINTK(GRN,   "!!!'%s' controller successfully probed!!!\n", 
                    i2s_dev->plat_data->dev_name);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

static int i2s_remove(struct platform_device *pdev)
{
    struct i2s_device * i2s_dev = platform_get_drvdata(pdev);    
    __PRINTK(RED, "Removing '%s' module.\n", i2s_dev->plat_data->dev_name);
    i2s_destroy_sys_iface(i2s_dev);
    i2s_destroy_dev_iface(i2s_dev);
    __PRINTK(RED, "Disable interrupts.\n");
	i2s_writel(i2s_dev->base, I2SC_IDR_OFF, -1);
    i2s_disable_dma(i2s_dev);
    i2s_disable_clk(i2s_dev);
    __PRINTK(RED, "!!!I2S module removed!!!\n");
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! Two mechanisms are provided :
 * 1) Find by name and #of_match_table that will be stored in kernel and passed back.
 * 2) Extract #id_table manually from module itself.
 */
static struct platform_driver i2s_driver = {
    .driver     = {
        .name       = "i2s",
        .of_match_table = of_match_ptr(i2s_dev_tree_ids),
    },
    .id_table   = i2s_dev_types,
    .probe      = i2s_probe, ///< As soon as #platform_driver_register succed this call will be made.
    .remove     = i2s_remove,
};
/*--- End of function ----------------------------------------------------------------------------*/

/*! @name Initialization facilities
 * @{ */
static int __init i2s_mod_init(void)
{
    __PRINTK(GRN, "Initializing I2S driver.\r\n");
    /*! Platform drivers make themselves known to the kernel. */
    return platform_driver_register(&i2s_driver);
}
/*--- End of function ----------------------------------------------------------------------------*/

static void __exit i2s_mod_exit(void)
{
    platform_driver_unregister(&i2s_driver);    
    __PRINTK(RED, "Platform driver removed.\r\n"NORM);
}
/*! @} -------------------------------------------------------------------------------------------*/

module_init(i2s_mod_init);
module_exit(i2s_mod_exit);


MODULE_AUTHOR("microElk");
MODULE_DESCRIPTION("I2S driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:i2s");
