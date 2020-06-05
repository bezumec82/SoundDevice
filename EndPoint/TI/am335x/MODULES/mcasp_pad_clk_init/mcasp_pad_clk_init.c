#include "mcasp_pad_clk_init.h"



static const struct of_device_id mcasp_dev_tree_ids[] = {
    {
        .compatible = "ti,am33xx-mcasp-audio",
        .data = NULL,
    },
    {
        .compatible = "ti,am33xx-mcasp-audio",
        .data = NULL,
    },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, mcasp_dev_tree_ids);
/*--- End of data declaration --------------------------------------------------------------------*/

static const struct platform_device_id mcasp_dev_types[] = {
    {
        .name = "am33xx-mcasp-audio",
    },
    {
        .name = "am33xx-mcasp-audio",
    },
    { /* sentinel */ }
};
/*--- End of data declaration --------------------------------------------------------------------*/

static int mcasp_configure_clk (struct mcasp_device * mcasp_dev)
{
    const char * clk_name = mcasp_dev->clk_name;
    __PRINTK(YEL, "Starting clock for controller '%s'.\n", mcasp_dev->pdev->name);
    of_property_read_string(mcasp_dev->dev_node, "clock-names", &clk_name);
    mcasp_dev->fck = devm_clk_get(mcasp_dev->dev, clk_name);
    if (IS_ERR(mcasp_dev->fck)) { __PRINTERR("Clock '%s' wasn't provided.\n", clk_name); return -1; }
    clk_prepare_enable(mcasp_dev->fck);
    __PRINTK(GRN, "Clock '%s' is enabled.\n", clk_name);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/
static void mcasp_disable_clk (struct mcasp_device * mcasp_dev)
{
    __PRINTK(RED, "Disabling clock for controller '%s'.\n", mcasp_dev->pdev->name);
    clk_disable_unprepare(mcasp_dev->fck);
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! McASP module probe function
 * @param pdev -    Device represented by the structure #platform_device
 *                  Will be passed by the kernel after device tree parsing.
 */
static int mcasp_probe(struct platform_device * pdev)
{
    struct mcasp_device * mcasp_dev;
    if(!pdev)
    {
        __PRINTK(RED, "ERROR : Platform device not provided.\n" );
        return -1;
    }
    __PRINTK(YEL, "Probing '%s' controller.\n", pdev->name);
    printk("!!! HZ = %i !!!\n", HZ);
    /// -# Allocate #mcasp_device structure
    mcasp_dev = devm_kzalloc(&pdev->dev, sizeof(struct mcasp_device), GFP_KERNEL);
    if (!mcasp_dev) {
        __PRINTK(RED, "!!! OUT OF MEMORY !!!.\n");
        return -ENOMEM; /* Out of memory */
    }
    /// -# Store struct #platform_device and struct #device passed
    /// by kernel in #mcasp_device structure.
    mcasp_dev->pdev     = pdev;
    mcasp_dev->dev      = &pdev->dev;
    mcasp_dev->dev_node = pdev->dev.of_node;    
    /// -# Extact clock data.
#if (1)
    if ( mcasp_configure_clk (mcasp_dev) )
    {
        __PRINTK(RED,   "Can't configure clock resources for controller '%s'.\n",
                        pdev->name);
        return -1;
    }
#endif
    pm_runtime_enable(&pdev->dev);
    pm_runtime_get(&pdev->dev);
    /// -# Extact IRQ # for the device.
    /// -# Save structure #mcasp_device as pdev->dev->driver_data for further use.
    /// Internally it is 'dev_set_drvdata(&pdev->dev, data)'.
    platform_set_drvdata(
                        pdev,       //struct platform_device
                        mcasp_dev   //Data to set (void *)
                        );
    __PRINTK(GRN,   "!!!'%s' controller successfully probed!!!\n",
                    mcasp_dev->pdev->name);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

static int mcasp_remove(struct platform_device *pdev)
{
    struct mcasp_device * mcasp_dev = platform_get_drvdata(pdev);
    __PRINTK(RED, "Removing '%s' module.\n", mcasp_dev->pdev->name);
    pm_runtime_disable(&pdev->dev);
    mcasp_disable_clk(mcasp_dev);
    __PRINTK(RED, "!!!'%s' module removed!!!\n", mcasp_dev->pdev->name);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

/*! Two mechanisms are provided :
 * 1) Find by name and #of_match_table that will be stored in kernel and passed back.
 * 2) Extract #id_table manually from module itself.
 */
static struct platform_driver mcasp_driver = {
    .driver = {
        .name           = "mcasp",
        .of_match_table = of_match_ptr(mcasp_dev_tree_ids),
    },
    .id_table   = mcasp_dev_types,
    .probe      = mcasp_probe,  ///< As soon as #platform_driver_register succed this call will be made.
    .remove = mcasp_remove,
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
