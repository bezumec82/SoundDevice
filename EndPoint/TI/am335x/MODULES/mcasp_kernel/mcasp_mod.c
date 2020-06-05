#include "mcasp_mod.h"

static const struct mcasp_mod_plat_data mcasp0_data = {
		.ctrlr_num	= 0,
		.dev_name	= "mcasp0",
};

static const struct mcasp_mod_plat_data mcasp1_data = {
		.ctrlr_num	= 1,
		.dev_name	= "mcasp1",
};

static const struct of_device_id mcasp_dev_tree_ids[] = {
	{
		.compatible = "ti,am33xx-mcasp-audio",
		.data = &mcasp0_data,
	},
	{
		.compatible = "ti,am33xx-mcasp-audio",
		.data = &mcasp1_data,
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, mcasp_dev_tree_ids);

static const struct platform_device_id mcasp_dev_types[] = {
	{
		.name = "am33xx-mcasp-audio",
		.driver_data = (unsigned long) &mcasp0_data,
	},
	{
		.name = "am33xx-mcasp-audio",
		.driver_data = (unsigned long) &mcasp1_data,
	},
	{ /* sentinel */ }
};

#if (McASP_INIT_REG)
static const struct regmap_config mcasp_regmap_config = {
	.reg_bits		= 32,				///< Number of bits in a register address, mandatory.

/*! The register address stride.
 * Valid register addresses are a multiple of this value.
 * If set to 0, a value of 1 will be used. */
	.reg_stride	= 4,

	.val_bits		= 32,				///< Number of bits in a register value, mandatory.
	.max_register	= McASP_RFIFOSTS,	///< Optional, specifies the maximum valid register address.
};
#endif
/*--- End of data declaration --------------------------------------------------------------------*/

/*! Extract platform data (void *) from #platform_device structure passed by kernel.
 * @param pdev - Device represented by structure #platform_device. Provided by kernel.
 */
static inline const struct mcasp_mod_plat_data * __init mcasp_mod_get_driver_data(struct platform_device * pdev)
{
	const struct of_device_id * match;

	__PRINTK(YEL, "Extacting platform data for McASP module.\n");
	/// -# If associated device tree node #device_node is present.
	if (pdev->dev.of_node) {
		__PRINTK(YEL, "Extacting platform from 'platform_device' passed by kernel.\n");
		/// -# Look if a #device_node has a matching of_match structure.
		match = of_match_node(
								mcasp_dev_tree_ids,	//Array of of device match structures to search in.
								pdev->dev.of_node	//The of device structure to match against.
								);
		if (match == NULL) {
			__PRINTK(RED, "No platform data provided.\n");
			return NULL;
		}
		__PRINTK(GRN,	"Platform data for '%s' controller successfully extacted.\n",
						((struct mcasp_mod_plat_data *)match->data)->dev_name);
		return (struct mcasp_mod_plat_data *)match->data;
	}
	__PRINTK(RED, "No platform data provided. Extracting data from module itself.\n");
	/// -# If no platfrom data provided, extract
	/// platform_device-> * platform_device_id-> kernel_ulong_t driver_data
	/// from driver itself. #of_device_id provided in #ssc_driver.
	return (struct mcasp_mod_plat_data *)platform_get_device_id(pdev)->driver_data;
}
/*--- End of function ----------------------------------------------------------------------------*/

static int mcasp_configure_clk (struct mcasp_device * mcasp_dev, struct platform_device * pdev)
{
	__PRINTK(YEL, "Starting clock for controller '%s'.\n", mcasp_dev->plat_data->dev_name);
	mcasp_dev->fck = devm_clk_get(&pdev->dev,"mcasp0_fck");
	if (IS_ERR(mcasp_dev->fck)) { __PRINTK(RED, "No peripheral clock provided.\n"); return -ENXIO; }
	clk_prepare_enable(mcasp_dev->fck);
	return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/
static void mcasp_disable_clk (struct mcasp_device * mcasp_dev)
{
	__PRINTK(RED, "Disabling clock for controller '%s'.\n", mcasp_dev->plat_data->dev_name);
	clk_disable_unprepare(mcasp_dev->fck);
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! McASP module probe function
 * @param pdev -	Device represented by the structure #platform_device
 *					Will be passed by the kernel after device tree parsing.
 */
static int mcasp_probe(struct platform_device * pdev)
{
	struct mcasp_device *	mcasp_dev;
#if (McASP_INIT_REG)
	struct regmap *			regmap;
#endif

	if(!pdev)
	{
		__PRINTK(RED, "ERROR : Platform device not provided.\n" );
		return -1;
	}
	__PRINTK(YEL, "Probing '%s' controller.\n", pdev->name);
	printk("!!! HZ = %i !!!\n", HZ);
	/// -# Allocate #mcasp_device structure
#if (1)
	mcasp_dev = devm_kzalloc(&pdev->dev, sizeof(struct mcasp_device), GFP_KERNEL);
	//ssc_dev = kzalloc(sizeof(struct ssc_device), GFP_KERNEL);
	if (!mcasp_dev) {
		__PRINTK(RED, "!!! OUT OF MEMORY !!!.\n");
		return -ENOMEM; /* Out of memory */
	}
#endif
	mcasp_dev->plat_data = mcasp_mod_get_driver_data(pdev);
	if (!mcasp_dev->plat_data)
	{
		__PRINTK(RED, "ERROR : Can't extract platform data.\n" );
		return -1;
	}
	/// -# Store struct #platform_device and struct #device passed
	/// by kernel in #mcasp_device structure.
	mcasp_dev->pdev		= pdev;
	mcasp_dev->dev		= &pdev->dev;
	mcasp_dev->dev_node = pdev->dev.of_node;
	/// -# Extract data about registers.
#if (1)
	mcasp_dev->cfg_regs = platform_get_resource	(				//return struct resource
												pdev,			//struct platform_device
												IORESOURCE_MEM,	//resource type
												0				//resource index
												);
	mcasp_dev->dat_regs = platform_get_resource	(				//return struct resource
												pdev,			//struct platform_device
												IORESOURCE_MEM,	//resource type
												1				//resource index
												);
	if ((!mcasp_dev->cfg_regs)||(!mcasp_dev->dat_regs))
	{
		__PRINTK(RED, "Can't extract register data from the device tree.\n");
		return -1;
	}
	__PRINTK(GRN,	"'%s' controller has :\n"
					"%39s cfg. IO memory region : 0x%lx - 0x%lx.\n"
					"%39s dat. IO memory region : 0x%lx - 0x%lx.\n",
					mcasp_dev->plat_data->dev_name,
					"",
					(long unsigned int)mcasp_dev->cfg_regs->start,
					(long unsigned int)mcasp_dev->cfg_regs->end,
					"",
					(long unsigned int)mcasp_dev->dat_regs->start,
					(long unsigned int)mcasp_dev->dat_regs->end
					);
	/// -# Check, request region, and ioremap resource.
	mcasp_dev->cfg_base = devm_ioremap_resource	(
												&pdev->dev,			//generic device to handle the resource for (struct device)
												mcasp_dev->cfg_regs	//resource to be handled (struct resource)
												);
	if (IS_ERR(mcasp_dev->cfg_base)){
		__PRINTK(RED,	"Can't request region : 0x%lx - 0x%lx for '%s' controller.\n",
						(long unsigned int)mcasp_dev->cfg_regs->start,
						(long unsigned int)mcasp_dev->cfg_regs->end,
						mcasp_dev->plat_data->dev_name
						);
		return PTR_ERR(mcasp_dev->cfg_base);
	}	
	__PRINTK(GRN,	"IO cfg. region (0x%p ... ) for '%s' controller successfully remapped.\n",
					mcasp_dev->cfg_base, mcasp_dev->plat_data->dev_name);
	/// -# Initialise managed register map.
#if (McASP_INIT_REG)
	regmap = devm_regmap_init_mmio	(
									&pdev->dev,				//Device that will be interacted with.
									mcasp_dev->cfg_base,	//Pointer to memory-mapped IO region.
									&mcasp_regmap_config	//Configuration for register map
									);
	if (IS_ERR(regmap))
	{
		__PRINTK(RED,	"Can't initialize IO region (0x%p ... ) for McASP module.\n", 
						mcasp_dev->cfg_base );
		return PTR_ERR(regmap);
	} else {
		__PRINTK(GRN,	"IO region (0x%p ... ) for '%s' controller successfully initialized.\n",
						mcasp_dev->cfg_base, mcasp_dev->plat_data->dev_name);
	}
#endif
#endif
	
	/// -# Extact clock data.
#if (1)
	if (mcasp_configure_clk (mcasp_dev, pdev))
	{
		__PRINTK(RED,	"Can't configure clock resources for controller '%s'.\n",
						mcasp_dev->plat_data->dev_name);
		return -ENXIO;
	}
#endif
	pm_runtime_enable(&pdev->dev);
	pm_runtime_get(&pdev->dev);
	/// -# Extact IRQ # for the device.
#if (1)
	mcasp_dev->rx_irq = platform_get_irq_byname(pdev, "rx");
	mcasp_dev->tx_irq = platform_get_irq_byname(pdev, "tx");
	if ((!mcasp_dev->rx_irq)||(!mcasp_dev->tx_irq))
	{
		__PRINTK(RED,	"No data about IRQ provided for controller '%s'.\n",
						mcasp_dev->plat_data->dev_name);
		return -ENXIO;
	}
	__PRINTK(GRN,	"IRQ data for McASP module successfully extracted.\n"
					"%39s TX IRQ #%i. RX IRQ #%i.\n", 
					"", mcasp_dev->rx_irq, mcasp_dev->tx_irq);
#endif
	/// -# Save structure #mcasp_device as pdev->dev->driver_data for further use.
	/// Internally it is 'dev_set_drvdata(&pdev->dev, data)'.
	platform_set_drvdata(
						pdev,		//struct platform_device
						mcasp_dev	//Data to set (void *)
						);

	if (mcasp_configure_dma(mcasp_dev))
	{
		__PRINTK(RED,	"Can't configure DMA for controller '%s'.\n",
						mcasp_dev->plat_data->dev_name);
		return -ENXIO;
	}
	__PRINTK(GRN,	"DMA for controller '%s' successfully configured.\n",
					mcasp_dev->plat_data->dev_name);
	mcasp_create_sys_iface(mcasp_dev);
	mcasp_create_dev_iface(mcasp_dev);
	__PRINTK(GRN,	"!!!'%s' controller successfully probed!!!\n",
					mcasp_dev->plat_data->dev_name);
	return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

static int mcasp_remove(struct platform_device *pdev)
{
	struct mcasp_device * mcasp_dev = platform_get_drvdata(pdev);
	__PRINTK(RED, "Removing '%s' module.\n", mcasp_dev->plat_data->dev_name);
	mcasp_destroy_sys_iface(mcasp_dev);
	mcasp_destroy_dev_iface(mcasp_dev);
	mcasp_disable_dma(mcasp_dev);
	mcasp_disable_clk(mcasp_dev);
	pm_runtime_disable(&pdev->dev);
	__PRINTK(RED, "!!!'%s' module removed!!!\n", mcasp_dev->plat_data->dev_name);
	return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! Two mechanisms are provided :
 * 1) Find by name and #of_match_table that will be stored in kernel and passed back.
 * 2) Extract #id_table manually from module itself.
 */
static struct platform_driver mcasp_driver = {
	.driver	= {
		.name			= "mcasp",
		.of_match_table = of_match_ptr(mcasp_dev_tree_ids),
	},
	.id_table	= mcasp_dev_types,
	.probe		= mcasp_probe,	///< As soon as #platform_driver_register succed this call will be made.
	.remove	= mcasp_remove,
};
/*--- End of function ----------------------------------------------------------------------------*/

/*! @name Initialization facilities
 * @{ */
static int __init mcasp_mod_init(void)
{
	__PRINTK(GRN, "Initializing McASP driver.\r\n");
	/*! Platform drivers make themselves known to the kernel. */
	return platform_driver_register(&mcasp_driver);
}
/*--- End of function ----------------------------------------------------------------------------*/

static void __exit mcasp_mod_exit(void)
{
	platform_driver_unregister(&mcasp_driver);
	__PRINTK(RED, "McASP driver removed.\r\n"NORM);
}
/*! @} -------------------------------------------------------------------------------------------*/
module_init(mcasp_mod_init);
module_exit(mcasp_mod_exit);

MODULE_AUTHOR("microElk");
MODULE_DESCRIPTION("McASP driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:mcasp");
