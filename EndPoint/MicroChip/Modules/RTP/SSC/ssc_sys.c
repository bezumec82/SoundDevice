#include "ssc_mod.h"

/*!
Test:
cd /sys/bus/platform/devices/f8004000.ssc0/regs
grep -H . .
 */

/* Functions prototypes */
static ssize_t ssc_store    (
                            struct device * dev,
                            struct device_attribute * attr,
                            const char * buf, 
                            size_t count
                            );
static ssize_t ssc_show     (
                            struct device * dev,
                            struct device_attribute * attr,
                            char * buf
                            ); 
/*--- End of functions prototypes ----------------------------------------------------------------*/

/* Data declaration */
static struct device_attribute dev_attr_SSC_CR       = { .attr = { .name = "SSC_CR"  ,  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Control Register
static struct device_attribute dev_attr_SSC_CMR      = { .attr = { .name = "SSC_CMR" ,  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Clock Mode Register
static struct device_attribute dev_attr_SSC_RCMR     = { .attr = { .name = "SSC_RCMR",  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Receive Clock Mode Register
static struct device_attribute dev_attr_SSC_RFMR     = { .attr = { .name = "SSC_RFMR",  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Receive Frame Mode Register
static struct device_attribute dev_attr_SSC_TCMR     = { .attr = { .name = "SSC_TCMR",  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Transmit Clock Mode Register
static struct device_attribute dev_attr_SSC_TFMR     = { .attr = { .name = "SSC_TFMR",  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Transmit Frame Mode Register
static struct device_attribute dev_attr_SSC_RHR      = { .attr = { .name = "SSC_RHR" ,  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Receive Holding Register
static struct device_attribute dev_attr_SSC_THR      = { .attr = { .name = "SSC_THR" ,  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Transmit Holding Register
static struct device_attribute dev_attr_SSC_RSHR     = { .attr = { .name = "SSC_RSHR",  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Receive Synchronization Holding Register
static struct device_attribute dev_attr_SSC_TSHR     = { .attr = { .name = "SSC_TSHR",  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Transmit Synchronization Holding Register//
static struct device_attribute dev_attr_SSC_RC0R     = { .attr = { .name = "SSC_RC0R",  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Receive Compare 0 Register
static struct device_attribute dev_attr_SSC_RC1R     = { .attr = { .name = "SSC_RC1R",  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Receive Compare 1 Register
static struct device_attribute dev_attr_SSC_SR       = { .attr = { .name = "SSC_SR"  ,  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Status Register
static struct device_attribute dev_attr_SSC_IER      = { .attr = { .name = "SSC_IER" ,  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Interrupt Enable Register
static struct device_attribute dev_attr_SSC_IDR      = { .attr = { .name = "SSC_IDR" ,  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Interrupt Disable Register
static struct device_attribute dev_attr_SSC_IMR      = { .attr = { .name = "SSC_IMR" ,  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Interrupt Mask Register
static struct device_attribute dev_attr_SSC_WPMR     = { .attr = { .name = "SSC_WPMR",  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Write Protection Mode Register
static struct device_attribute dev_attr_SSC_WPSR     = { .attr = { .name = "SSC_WPSR",  .mode = S_IRUGO | S_IWUSR, }, .show = ssc_show, .store = ssc_store, };   ///SSC Write Protection Status Register

/* Declare attributes table (listing in fact sysfs files for driver) */
static struct attribute * ssc_attrs[] = {
    &dev_attr_SSC_CR.attr,      ///SSC Control Register
    &dev_attr_SSC_CMR.attr,     ///SSC Clock Mode Register                                
    &dev_attr_SSC_RCMR.attr,    ///SSC Receive Clock Mode Register
    &dev_attr_SSC_RFMR.attr,    ///SSC Receive Frame Mode Register
    &dev_attr_SSC_TCMR.attr,    ///SSC Transmit Clock Mode Register
    &dev_attr_SSC_TFMR.attr,    ///SSC Transmit Frame Mode Register                                
    &dev_attr_SSC_RHR.attr,     ///SSC Receive Holding Register
    &dev_attr_SSC_THR.attr,     ///SSC Transmit Holding Register                                
    &dev_attr_SSC_RSHR.attr,    ///SSC Receive Synchronization Holding Register
    &dev_attr_SSC_TSHR.attr,    ///SSC Transmit Synchronization Holding Register                                
    &dev_attr_SSC_RC0R.attr,    ///SSC Receive Compare 0 Register
    &dev_attr_SSC_RC1R.attr,    ///SSC Receive Compare 1 Register                                
    &dev_attr_SSC_SR.attr,      ///SSC Status Register                                
    &dev_attr_SSC_IER.attr,     ///SSC Interrupt Enable Register
    &dev_attr_SSC_IDR.attr,     ///SSC Interrupt Disable Register
    &dev_attr_SSC_IMR.attr,     ///SSC Interrupt Mask Register                                
    &dev_attr_SSC_WPMR.attr,    ///SSC Write Protection Mode Register
    &dev_attr_SSC_WPSR.attr,    ///SSC Write Protection Status Register
    NULL
};

/* Declare attribute group (specifying in fact sysfs directory for your driver) */
static struct attribute_group ssc_group = {
    .name = "regs",
    .attrs = ssc_attrs,
};
/*--- End of data declaration --------------------------------------------------------------------*/

/*! This function creates sys_fs interface
 * and exposes all internal SSC peripheral registers to it.
 */
int ssc_create_sys_iface(struct ssc_device * ssc_dev)
{
    int ret_val = 0;   

    ret_val = sysfs_create_group(&ssc_dev->pdev->dev.kobj, &ssc_group);
    if (ret_val){
        __PRINTK(RED, "'sysfs' group creation failed.\n");    
        return ret_val;
    } else {
        __PRINTK(GRN, "'sysfs' group for SSC module successfully created.\n");  
        __PRINTK(
                CYN,"Look files in folder: "
                "/sys/bus/platform/devices/%x.ssc%i/regs\n", 
                ssc_dev->mem_start, ssc_dev->ctrlr_num
                );
    }
    /// Store attribute group.
    ssc_dev->attr_group = &ssc_group;
    return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

void attr_finder    (
                    struct ssc_device * ssc_dev,
                    struct device_attribute * attr,
                    void __iomem ** reg_addr,
                    char * reg_name
                    )
{
    if      (attr == (struct device_attribute *)&dev_attr_SSC_CR.attr)     {   *reg_addr = ssc_dev->base + SSC_CR_OFFSET         ; strcpy(reg_name, "SSC_CR"  );   }   ///SSC Control Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_CMR.attr)    {   *reg_addr = ssc_dev->base + SSC_CMR_OFFSET        ; strcpy(reg_name, "SSC_CMR" );   }   ///SSC Clock Mode Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_RCMR.attr)   {   *reg_addr = ssc_dev->base + SSC_RCMR_OFFSET       ; strcpy(reg_name, "SSC_RCMR");   }   ///SSC Receive Clock Mode Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_RFMR.attr)   {   *reg_addr = ssc_dev->base + SSC_RFMR_OFFSET       ; strcpy(reg_name, "SSC_RFMR");   }   ///SSC Receive Frame Mode Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_TCMR.attr)   {   *reg_addr = ssc_dev->base + SSC_TCMR_OFFSET       ; strcpy(reg_name, "SSC_TCMR");   }   ///SSC Transmit Clock Mode Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_TFMR.attr)   {   *reg_addr = ssc_dev->base + SSC_TFMR_OFFSET       ; strcpy(reg_name, "SSC_TFMR");   }   ///SSC Transmit Frame Mode Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_RHR.attr)    {   *reg_addr = ssc_dev->base + SSC_RHR_OFFSET        ; strcpy(reg_name, "SSC_RHR" );   }   ///SSC Receive Holding Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_THR.attr)    {   *reg_addr = ssc_dev->base + SSC_THR_OFFSET        ; strcpy(reg_name, "SSC_THR" );   }   ///SSC Transmit Holding Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_RSHR.attr)   {   *reg_addr = ssc_dev->base + SSC_RSHR_OFFSET       ; strcpy(reg_name, "SSC_RSHR");   }   ///SSC Receive Synchronization Holding Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_TSHR.attr)   {   *reg_addr = ssc_dev->base + SSC_TSHR_OFFSET       ; strcpy(reg_name, "SSC_TSHR");   }   ///SSC Transmit Synchronization Holding Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_RC0R.attr)   {   *reg_addr = ssc_dev->base + SSC_RC0R_OFFSET       ; strcpy(reg_name, "SSC_RC0R");   }   ///SSC Receive Compare 0 Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_RC1R.attr)   {   *reg_addr = ssc_dev->base + SSC_RC1R_OFFSET       ; strcpy(reg_name, "SSC_RC1R");   }   ///SSC Receive Compare 1 Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_SR.attr)     {   *reg_addr = ssc_dev->base + SSC_SR_OFFSET         ; strcpy(reg_name, "SSC_SR"  );   }   ///SSC Status Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_IER.attr)    {   *reg_addr = ssc_dev->base + SSC_IER_OFFSET        ; strcpy(reg_name, "SSC_IER" );   }   ///SSC Interrupt Enable Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_IDR.attr)    {   *reg_addr = ssc_dev->base + SSC_IDR_OFFSET        ; strcpy(reg_name, "SSC_IDR" );   }   ///SSC Interrupt Disable Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_IMR.attr)    {   *reg_addr = ssc_dev->base + SSC_IMR_OFFSET        ; strcpy(reg_name, "SSC_IMR" );   }   ///SSC Interrupt Mask Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_WPMR.attr)   {   *reg_addr = ssc_dev->base + SSC_WPMR_OFFSET       ; strcpy(reg_name, "SSC_WPMR");   }   ///SSC Write Protection Mode Register
    else if (attr == (struct device_attribute *)&dev_attr_SSC_WPSR.attr)   {   *reg_addr = ssc_dev->base + SSC_WPSR_OFFSET       ; strcpy(reg_name, "SSC_WPSR");   }   ///SSC Write Protection Status Register    
    else {
        strcpy(reg_name, "!!ERROR!!");
        reg_addr = NULL;
        __PRINTK(RED, "Can't find attribute file.n");
        
    }
    return; 
}

/* Show/store methods */
static ssize_t ssc_store    (
                            struct device * dev,
                            struct device_attribute * attr,
                            const char * buf,
                            size_t count
                            ) 
{
    int                 value;
    struct ssc_device * ssc_dev;
    void __iomem *      reg_addr        = NULL;
    char                reg_name[16]    = { 0 };

    ssc_dev = dev_get_drvdata(dev);
    if (!ssc_dev){
        __PRINTK(RED, "Can't extract SSC module data.\r\n");
        return 0;
    }
    attr_finder(ssc_dev, attr, &reg_addr, reg_name);
    value = simple_strtol(buf, NULL, 10);
#if SSC_SYS_DEBUG_SEVERE
    __PRINTK(CYN, "Writing 0x%x to the register %-10s (0x%p).\n", value, reg_name, reg_addr );     
#endif
    if (reg_addr!=NULL) { 
        writel(value, reg_addr); 
#if SSC_SYS_DEBUG_SEVERE
        __PRINTK(CYN, "Success.\n");
#endif
        return count;
    } else {
        __PRINTK(RED, "Failure.\n");
        return count;
    }
}
/*--- End of function ----------------------------------------------------------------------------*/

static ssize_t ssc_show     (
                            struct device * dev,
                            struct device_attribute * attr,
                            char * buf
                            )
{
    int                 value;
    struct ssc_device * ssc_dev;
    void __iomem *      reg_addr = NULL;
    char                reg_name[16];

    ssc_dev = dev_get_drvdata(dev);
    if (!ssc_dev) {
        __PRINTK(RED, "Can't extract SSC module data.\r\n");
        return 0;
    }
    attr_finder(ssc_dev, attr, &reg_addr, reg_name);
#if SSC_SYS_DEBUG_SEVERE
    __PRINTK(CYN, "Reading from the register %-10s (0x%p).\n", reg_name, reg_addr );     
#endif    
    if (reg_addr!=NULL) { 
        value = readl(reg_addr); 
#if SSC_SYS_DEBUG_SEVERE
        __PRINTK(CYN, "%-10s (0x%p) register value : 0x%x.\n", reg_name, reg_addr, value );    
#endif
        sprintf (buf, "%d", value);        
        return strlen(buf);
    } else {
        __PRINTK(RED, "Failure.\n");
        return 0;
    }
}
/*--- End of function ----------------------------------------------------------------------------*/


