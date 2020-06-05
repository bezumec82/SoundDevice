#include "i2s_mod.h"

#define TIME_QUANTUM_MS     20
#define BANDWIDTH           ( (16000 * 16 * 2) / 8 ) /* bytes per second */
#define MEM_QUANTUM_BYTES   (20 * BANDWIDTH / 1000 )



/*! Signature :
 * ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
 */
ssize_t i2s_read  (
                    struct file *   filp, 
                    char __user *   buf, 
                    size_t          count,
                    loff_t *        f_pos
                    )
{
#if (0)
    ssize_t             ret_val = 0;
    struct i2s_device * i2s_dev = filp->private_data;
    char msg[] = "Read\n";
    __PRINTK(CYN, "Reading device %s.\n", i2s_dev->plat_data->dev_name);

    if (mutex_lock_interruptible(&i2s_dev->mutex)) { return -ERESTARTSYS; }
    
    count = sizeof(msg);
    if (copy_to_user(buf, msg, count)) 
    {
		ret_val = -EFAULT;
		goto out;
	}
    
	*f_pos += count;
	ret_val = count;
out:
    mutex_unlock(&i2s_dev->mutex);
    return ret_val;
#else
    struct i2s_device * i2s_dev = filp->private_data;
    __PRINTK(CYN, "Reading device %s.\n", i2s_dev->plat_data->dev_name);
    return 0;
#endif
}
/*--- End of function ----------------------------------------------------------------------------*/

int i2s_open (struct inode * inode_ptr, struct file * filp)
{   
    struct i2s_device * i2s_dev = container_of(inode_ptr->i_cdev, struct i2s_device, cdev);
    filp->private_data = i2s_dev; /* for other methods */
    __PRINTK(CYN, "Opening device %s.\n", i2s_dev->plat_data->dev_name);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

int i2s_release (struct inode * inode_ptr, struct file * filp)
{
    struct i2s_device * i2s_dev = container_of(inode_ptr->i_cdev, struct i2s_device, cdev);
    __PRINTK(CYN, "Releasing device %s.\n", i2s_dev->plat_data->dev_name);   
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

static const struct file_operations  i2s_dev_fops = {
	.owner      = THIS_MODULE,
	.read       = i2s_read,
	.write      = i2s_write,
    .open       = i2s_open,
    .release    = i2s_release,
};

#define I2SC_FIRST_MINOR 0
#define I2SC_DEVICES     1


int i2s_create_dev_iface(struct i2s_device * i2s_dev)
{
    int ret_val = 0;
    struct device * tmp_dev; 
    
     __PRINTK(YEL,"Creating 'dev' interface for '%s' controller.\n", i2s_dev->plat_data->dev_name);   
     
    __PRINTK(MAG,"Allocating driver numbers.\n");   
#if (1)
    ret_val = alloc_chrdev_region(&i2s_dev->dev_num, I2SC_FIRST_MINOR, I2SC_DEVICES, &i2s_dev->plat_data->dev_name[0]);
#else
    i2s_dev->dev_num = 200;
    ret_val = register_chrdev_region(
                                    i2s_dev->dev_num,               /* output-only parameter that will, on successful completion,  hold  the  first  number  in  your  allocated  range */
                                    I2SC_DEVICES,                   /* total number of contiguous device numbers you are requesting */
                                    i2s_dev->plat_data->dev_name    /* name of the device that should be associatedwith this number range */
                                    );
#endif
    if (ret_val < 0)
    {
        __PRINTK(RED, "Can't allocate number for controller %s.\n", i2s_dev->plat_data->dev_name);
        return ret_val;
    }
    mutex_init(&i2s_dev->mutex);
    
    __PRINTK(MAG,"Creating char device.\n");        
	cdev_init(&i2s_dev->cdev, &i2s_dev_fops);
    i2s_dev->cdev.owner = THIS_MODULE;
	ret_val = cdev_add  (
                        &i2s_dev->cdev,     /* struct cdev *  */
                        i2s_dev->dev_num,   /* the first device number for which this device is responsible */ 
                        I2SC_DEVICES        /* the number of consecutive minor numbers corresponding to this device */
                        );
	if (ret_val)
    {
		__PRINTK(RED,   "ERROR : Can't add cdev for '%s' controller. ret_val = %d.\n",
                        i2s_dev->plat_data->dev_name, ret_val);
        goto unregister_chrdev;
    }
    

    __PRINTK(MAG,"Creating class.\n");
    i2s_dev->i2s_class = class_create(THIS_MODULE, i2s_dev->plat_data->dev_name);
    if (IS_ERR(i2s_dev->i2s_class)) {
		ret_val = PTR_ERR(i2s_dev->i2s_class);
        __PRINTK(RED,"ERROR : Can't create 'i2s' class. ret_val = %d.\n", ret_val);
        goto delete_cdev;
	}
    __PRINTK(MAG,"Creating device %s.\n", i2s_dev->plat_data->dev_name);  
    /* A "dev" file will be created, showing the dev_t for the device, if the dev_t is not 0,0. */
    tmp_dev = device_create (
                            i2s_dev->i2s_class,             /* pointer to the struct class that this device should be registered to */
                            NULL,                           /* pointer to the parent struct device of this new device */
                            i2s_dev->dev_num,               /* the 'dev_t' for the char device to be added */
                            (void *)i2s_dev,                /* the data to be added to the device for callbacks */
                            i2s_dev->plat_data->dev_name    /* string for the device's name */
                            );
    if (IS_ERR(tmp_dev)) 
    {
		ret_val = PTR_ERR(tmp_dev);
        __PRINTK(RED,   "ERROR : Can't create '%s' device. ret_val = %d.\n", 
                        i2s_dev->plat_data->dev_name, ret_val);
        goto destroy_class;
	}
                  
    __PRINTK(GRN,   "Char device '/dev/%s' successfully initialized. Major : %i. Minor : %i.\n", 
                    i2s_dev->plat_data->dev_name, MAJOR(i2s_dev->dev_num), MINOR(i2s_dev->dev_num));    
          
    return 0;
     
destroy_class:
    class_destroy(i2s_dev->i2s_class);
delete_cdev:
    cdev_del(&i2s_dev->cdev);
unregister_chrdev:
    unregister_chrdev_region(i2s_dev->dev_num, I2SC_DEVICES);  
    return -1;
    
}
/*--- End of function ----------------------------------------------------------------------------*/

int i2s_destroy_dev_iface(struct i2s_device * i2s_dev)
{   
    mutex_unlock(&i2s_dev->mutex);
    if (i2s_dev->i2s_class != NULL)
    {
        __PRINTK(RED, "Removing 'dev' interface.\n");
        device_destroy(i2s_dev->i2s_class, i2s_dev->dev_num);
        class_destroy(i2s_dev->i2s_class);
    }
    cdev_del(&i2s_dev->cdev);
    unregister_chrdev_region(i2s_dev->dev_num, I2SC_DEVICES);
#if DMA_SIMPLE

#elif DMA_CIRC_BUF

#endif
    
    
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

                    
