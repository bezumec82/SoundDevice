#include "i2s_mod.h"

/*!
Test:
cd /sys/bus/platform/devices/f8004000.i2s0/regs
grep -H . .
 */

/* Functions prototypes */
static ssize_t i2s_store    (
                            struct device * dev,
                            struct device_attribute * attr,
                            const char * buf, 
                            size_t count
                            );
static ssize_t i2s_show     (
                            struct device * dev,
                            struct device_attribute * attr,
                            char * buf
                            ); 
/*--- End of functions prototypes ----------------------------------------------------------------*/

/* Data declaration */
static struct device_attribute dev_attr_I2SC_CR  = { .attr = { .name = "I2SC_CR" ,  .mode = S_IRUGO | S_IWUSR, }, .show = i2s_show, .store = i2s_store, };   ///< I2SC Control Register
static struct device_attribute dev_attr_I2SC_MR  = { .attr = { .name = "I2SC_MR" ,  .mode = S_IRUGO | S_IWUSR, }, .show = i2s_show, .store = i2s_store, };   ///< I2SC Mode Register
static struct device_attribute dev_attr_I2SC_SR  = { .attr = { .name = "I2SC_SR" ,  .mode = S_IRUGO | S_IWUSR, }, .show = i2s_show, .store = i2s_store, };   ///< I2SC Status Register
static struct device_attribute dev_attr_I2SC_SCR = { .attr = { .name = "I2SC_SCR",  .mode = S_IRUGO | S_IWUSR, }, .show = i2s_show, .store = i2s_store, };   ///< I2SC Status Clear Register
static struct device_attribute dev_attr_I2SC_SSR = { .attr = { .name = "I2SC_SSR",  .mode = S_IRUGO | S_IWUSR, }, .show = i2s_show, .store = i2s_store, };   ///< I2SC Status Set Register
static struct device_attribute dev_attr_I2SC_IER = { .attr = { .name = "I2SC_IER",  .mode = S_IRUGO | S_IWUSR, }, .show = i2s_show, .store = i2s_store, };   ///< I2SC Interrupt Enable Register
static struct device_attribute dev_attr_I2SC_IDR = { .attr = { .name = "I2SC_IDR",  .mode = S_IRUGO | S_IWUSR, }, .show = i2s_show, .store = i2s_store, };   ///< I2SC Interrupt Disable Register
static struct device_attribute dev_attr_I2SC_IMR = { .attr = { .name = "I2SC_IMR",  .mode = S_IRUGO | S_IWUSR, }, .show = i2s_show, .store = i2s_store, };   ///< I2SC Interrupt Mask Register
static struct device_attribute dev_attr_I2SC_RHR = { .attr = { .name = "I2SC_RHR",  .mode = S_IRUGO | S_IWUSR, }, .show = i2s_show, .store = i2s_store, };   ///< I2SC Receiver Holding Register
static struct device_attribute dev_attr_I2SC_THR = { .attr = { .name = "I2SC_THR",  .mode = S_IRUGO | S_IWUSR, }, .show = i2s_show, .store = i2s_store, };   ///< I2SC Transmitter Holding Register


/* Declare attributes table (listing in fact sysfs files for driver) */
static struct attribute * i2s_attrs[] = {
    &dev_attr_I2SC_CR.attr  , ///< I2SC Control Register
    &dev_attr_I2SC_MR.attr  , ///< I2SC Mode Register       
    &dev_attr_I2SC_SR.attr  , ///< I2SC Status Register
    &dev_attr_I2SC_SCR.attr , ///< I2SC Status Clear Register
    &dev_attr_I2SC_SSR.attr , ///< I2SC Status Set Register
    &dev_attr_I2SC_IER.attr , ///< I2SC Interrupt Enable Register                
    &dev_attr_I2SC_IDR.attr , ///< I2SC Interrupt Disable Register
    &dev_attr_I2SC_IMR.attr , ///< I2SC Interrupt Mask Register             
    &dev_attr_I2SC_RHR.attr , ///< I2SC Receiver Holding Register
    &dev_attr_I2SC_THR.attr , ///< I2SC Transmitter Holding Register   
    NULL /* sentinel */                          
};

/* Declare attribute group (specifying in fact sysfs directory for your driver) */
static struct attribute_group i2s_group = {
    .name = "regs",
    .attrs = i2s_attrs,
};
/*--- End of data declaration --------------------------------------------------------------------*/

/*! This function creates sys_fs interface
 * and exposes all internal I2S peripheral registers to it.
 */
int i2s_create_sys_iface(struct i2s_device * i2s_dev)
{
    int ret_val = 0;   
     __PRINTK(YEL,  "Creating 'sys' interface for '%s' controller.\n", 
                    i2s_dev->plat_data->dev_name);   
     
    if (i2s_dev->dev == NULL)
    {
        __PRINTK(RED, "ERROR : Can't create 'sysfs' group. 'dev' is NULL.\n");
        return -1; 
    }
    ret_val = sysfs_create_group(&i2s_dev->dev->kobj, &i2s_group);
    if (ret_val)
    {
        __PRINTK(RED, "'sysfs' group creation failed.\n");    
        return ret_val;
    } 
    else 
    {
        __PRINTK(GRN,   "'sysfs' group for '%s' controller successfully created.\n",
                        i2s_dev->plat_data->dev_name);  
        __PRINTK(
                CYN,"Look files in folder: "
                "/sys/bus/platform/devices/%x.i2s/regs\n", 
                i2s_dev->regs->start );
    }
    
    /// Store attribute group.
    i2s_dev->attr_group = &i2s_group;
    return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

void i2s_destroy_sys_iface(struct i2s_device * i2s_dev)
{
    if (i2s_dev->dev!=NULL)
    {
        __PRINTK(RED, "Removing 'sysfs' interface.\n");
        sysfs_remove_group(&i2s_dev->dev->kobj, i2s_dev->attr_group);
    }
}

void attr_finder    (
                    struct i2s_device * i2s_dev,
                    struct device_attribute * attr,
                    void __iomem ** reg_addr,
                    char * reg_name
                    )
{
    if      (attr == (struct device_attribute *)&dev_attr_I2SC_CR.attr)    {   *reg_addr = i2s_dev->base + I2SC_CR_OFF  ; strcpy(reg_name, "I2SC_CR" );   }   ///< I2SC Control Register
    else if (attr == (struct device_attribute *)&dev_attr_I2SC_MR.attr)    {   *reg_addr = i2s_dev->base + I2SC_MR_OFF  ; strcpy(reg_name, "I2SC_MR" );   }   ///< I2SC Mode Register       
    else if (attr == (struct device_attribute *)&dev_attr_I2SC_SR.attr)    {   *reg_addr = i2s_dev->base + I2SC_SR_OFF  ; strcpy(reg_name, "I2SC_SR" );   }   ///< I2SC Status Register
    else if (attr == (struct device_attribute *)&dev_attr_I2SC_SCR.attr)   {   *reg_addr = i2s_dev->base + I2SC_SCR_OFF ; strcpy(reg_name, "I2SC_SCR");   }   ///< I2SC Status Clear Register
    else if (attr == (struct device_attribute *)&dev_attr_I2SC_SSR.attr)   {   *reg_addr = i2s_dev->base + I2SC_SSR_OFF ; strcpy(reg_name, "I2SC_SSR");   }   ///< I2SC Status Set Register
    else if (attr == (struct device_attribute *)&dev_attr_I2SC_IER.attr)   {   *reg_addr = i2s_dev->base + I2SC_IER_OFF ; strcpy(reg_name, "I2SC_IER");   }   ///< I2SC Interrupt Enable Register   
    else if (attr == (struct device_attribute *)&dev_attr_I2SC_IDR.attr)   {   *reg_addr = i2s_dev->base + I2SC_IDR_OFF ; strcpy(reg_name, "I2SC_IDR");   }   ///< I2SC Interrupt Disable Register
    else if (attr == (struct device_attribute *)&dev_attr_I2SC_IMR.attr)   {   *reg_addr = i2s_dev->base + I2SC_IMR_OFF ; strcpy(reg_name, "I2SC_IMR");   }   ///< I2SC Interrupt Mask Register     
    else if (attr == (struct device_attribute *)&dev_attr_I2SC_RHR.attr)   {   *reg_addr = i2s_dev->base + I2SC_RHR_OFF ; strcpy(reg_name, "I2SC_RHR");   }   ///< I2SC Receiver Holding Register
    else if (attr == (struct device_attribute *)&dev_attr_I2SC_THR.attr)   {   *reg_addr = i2s_dev->base + I2SC_THR_OFF ; strcpy(reg_name, "I2SC_THR");   }   ///< I2SC Transmitter Holding Register

    else {
        strcpy(reg_name, "!!ERROR!!");
        reg_addr = NULL;
        __PRINTK(RED, "Can't find attribute file.n");        
    }
    return; 
}

/* Show/store methods */
static ssize_t i2s_store    (
                            struct device * dev,
                            struct device_attribute * attr,
                            const char * buf,
                            size_t count
                            ) 
{
    int                 value;
    struct i2s_device * i2s_dev;
    void __iomem *      reg_addr        = NULL;
    char                reg_name[16]    = { 0 };

    i2s_dev = dev_get_drvdata(dev);
    if (!i2s_dev){
        __PRINTK(RED, "Can't extract I2S module data.\r\n");
        return 0;
    }
    attr_finder(i2s_dev, attr, &reg_addr, reg_name);
    value = simple_strtol(buf, NULL, 10);
#if I2S_SYS_DEBUG_SEVERE
    __PRINTK(CYN, "Writing 0x%x to the register %-10s (0x%p).\n", value, reg_name, reg_addr );     
#endif
    if (reg_addr!=NULL) { 
        writel(value, reg_addr); 
#if I2S_SYS_DEBUG_SEVERE
        __PRINTK(CYN, "Success.\n");
#endif
        return count;
    } else {
        __PRINTK(RED, "Failure.\n");
        return count;
    }
}
/*--- End of function ----------------------------------------------------------------------------*/

static ssize_t i2s_show     (
                            struct device * dev,
                            struct device_attribute * attr,
                            char * buf
                            )
{
    int                 value;
    struct i2s_device * i2s_dev;
    void __iomem *      reg_addr = NULL;
    char                reg_name[16];

    i2s_dev = dev_get_drvdata(dev);
    if (!i2s_dev) {
        __PRINTK(RED, "Can't extract I2S module data.\r\n");
        return 0;
    }
    attr_finder(i2s_dev, attr, &reg_addr, reg_name);
#if I2S_SYS_DEBUG_SEVERE
    __PRINTK(CYN, "Reading from the register %-10s (0x%p).\n", reg_name, reg_addr );     
#endif    
    if (reg_addr!=NULL) { 
        value = readl(reg_addr); 
#if I2S_SYS_DEBUG_SEVERE
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


