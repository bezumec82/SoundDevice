#include "ssc_mod.h"

/*! Signature :
 * ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
 */
ssize_t ssc_read  (
                    struct file *   filp, 
                    char __user *   buf, 
                    size_t          count,
                    loff_t *        f_pos
                    )
{
#if (0)
    ssize_t             ret_val = 0;
    struct ssc_device * ssc_dev = filp->private_data;
    char msg[] = "Read\n";
    __PRINTK(CYN, "Reading device %s.\n", ssc_dev->dev_name);

    if (mutex_lock_interruptible(&ssc_dev->mutex)) { return -ERESTARTSYS; }
    
    count = sizeof(msg);
    if (copy_to_user(buf, msg, count)) 
    {
		ret_val = -EFAULT;
		goto out;
	}
    
	*f_pos += count;
	ret_val = count;
out:
    mutex_unlock(&ssc_dev->mutex);
    return ret_val;
#else
    struct ssc_device * ssc_dev = filp->private_data;
    __PRINTK(CYN, "Reading device %s.\n", ssc_dev->dev_name);
    return 0;
#endif
}
/*--- End of function ----------------------------------------------------------------------------*/

static void ssc_write_data_dma_callback(void * data)
{
	struct ssc_device * ssc_dev = (struct ssc_device *)data;
    __PRINTK(CYN, "DMA callback.\n");    
	dma_unmap_single(
                    ssc_dev->dev, 
                    sg_dma_address(&ssc_dev->dma.sg[0]),
                    ssc_dev->dma_buf_len, 
                    DMA_TO_DEVICE);

}

ssize_t ssc_write   (
                    struct file *       filp, 
                    const char __user * buf, 
                    size_t              count,
                    loff_t *            f_pos
                    )
{
    ssize_t                             ret_val     = 0;
    struct ssc_device *                 ssc_dev     = filp->private_data;
    dma_addr_t                          dma_addr;
    struct dma_async_tx_descriptor *    txdesc;
    
    __PRINTK(CYN, "Writing device %s.\n", ssc_dev->dev_name);
    if (mutex_lock_interruptible(&ssc_dev->mutex)) { return -ERESTARTSYS; }
    
    /* Get data from user */
    count > PAGE_SIZE ?
                        (ssc_dev->dma_buf_len = PAGE_SIZE) : (ssc_dev->dma_buf_len = count);
                        
    if (copy_from_user(ssc_dev->dma_buf, buf, ssc_dev->dma_buf_len)) 
    {
        __PRINTK(RED, "ERROR : Failed to get data from user.\n");
        ret_val = -EFAULT; goto out;
    }
        __PRINTK(CYN, "%u bytes copied from user.\n", ssc_dev->dma_buf_len);
    

    dma_addr = dma_map_single   (
                                ssc_dev->dev, 
                                ssc_dev->dma_buf, 
                                PAGE_SIZE,
                                DMA_TO_DEVICE);
	if (dma_mapping_error(ssc_dev->dev, dma_addr)) 
    {
		__PRINTK(RED, "ERROR : Failed to map DMA.\n");
        ret_val = -EFAULT;
		goto out;
	}   
    ssc_dev->dma_buf_len%2 ? (ssc_dev->dma_buf_len++) : ssc_dev->dma_buf_len ; //align to 2 bytes
    
    sg_dma_len      (&ssc_dev->dma.sg[0]) = ssc_dev->dma_buf_len;
    sg_dma_address  (&ssc_dev->dma.sg[0]) = dma_addr;
    
    txdesc = dmaengine_prep_slave_sg    (
                                        ssc_dev->dma.tx_ch, 
                                        ssc_dev->dma.sg, 
                                        1,
                                        DMA_MEM_TO_DEV,
                                        DMA_PREP_INTERRUPT | DMA_CTRL_ACK
                                        );
                                        
    txdesc->callback = ssc_write_data_dma_callback;
	txdesc->callback_param = ssc_dev;
    
    dmaengine_submit(txdesc);
	dma_async_issue_pending(ssc_dev->dma.tx_ch);
    
	*f_pos += count;
	ret_val = count;
out:
    mutex_unlock(&ssc_dev->mutex);
    return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

int ssc_open (struct inode * inode_ptr, struct file * filp)
{   
    
    struct ssc_device * ssc_dev = container_of(inode_ptr->i_cdev, struct ssc_device, cdev);
    filp->private_data = ssc_dev; /* for other methods */
    __PRINTK(CYN, "Opening device %s.\n", ssc_dev->dev_name);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

int ssc_release (struct inode * inode_ptr, struct file * filp)
{
    struct ssc_device * ssc_dev = container_of(inode_ptr->i_cdev, struct ssc_device, cdev);
    __PRINTK(CYN, "Releasing device %s.\n", ssc_dev->dev_name);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

static const struct file_operations  ssc_dev_fops = {
	.owner      = THIS_MODULE,
	.read       = ssc_read,
	.write      = ssc_write,
    .open       = ssc_open,
    .release    = ssc_release,
};

#define SSC_FIRST_MINOR 0
#define SSC_DEVICES     1


int ssc_create_dev_iface(struct ssc_device * ssc_dev)
{
    int ret_val = 0;
    struct device * tmp_dev; 
    
    ssc_dev->dma_buf = (void *)__get_free_pages(GFP_KERNEL, 0);
    if (!ssc_dev->dma_buf) {
        __PRINTK(RED,"Can't allocate memory.\n");   
		return -ENOMEM;
	}
    
    sprintf(ssc_dev->dev_name, "ssc%d", ssc_dev->ctrlr_num);
    __PRINTK(MAG,"Allocating driver numbers.\n");   

#if (1)
    ret_val = alloc_chrdev_region(&ssc_dev->dev_num, SSC_FIRST_MINOR, SSC_DEVICES, &ssc_dev->dev_name[0]);
#else
    ssc_dev->dev_num = 200;
    ret_val = register_chrdev_region(
                                    ssc_dev->dev_num,       /* output-only parameter that will, on successful completion,  hold  the  first  number  in  your  allocated  range */
                                    SSC_DEVICES,            /* total number of contiguous device numbers you are requesting */
                                    &ssc_dev->dev_name[0]   /* name of the device that should be associatedwith this number range */
                                    );
#endif
    if (ret_val < 0)
    {
        __PRINTK(RED, "Can't allocate number for controller %s.\n", ssc_dev->dev_name);
        return ret_val;
    }
    mutex_init(&ssc_dev->mutex);
    
    __PRINTK(MAG,"Creating char device.\n");        
	cdev_init(&ssc_dev->cdev, &ssc_dev_fops);
    ssc_dev->cdev.owner = THIS_MODULE;
	ret_val = cdev_add  (
                        &ssc_dev->cdev,     /* struct cdev *  */
                        ssc_dev->dev_num,   /* the first device number for which this device is responsible */ 
                        SSC_DEVICES         /* the number of consecutive minor numbers corresponding to this device */
                        );
	if (ret_val)
    {
		__PRINTK(RED,"ERROR : Can't add %s. ret_val = %d.\n", ssc_dev->dev_name, ret_val);
        unregister_chrdev_region(ssc_dev->dev_num, SSC_DEVICES);
        goto unregister_chrdev;
    }
    

    __PRINTK(MAG,"Creating class.\n");              
    ssc_dev->ssc_class = class_create(THIS_MODULE, "ssc");
    if (IS_ERR(ssc_dev->ssc_class)) {
		ret_val = PTR_ERR(ssc_dev->ssc_class);
        __PRINTK(RED,"ERROR : Can't create 'ssc' class. ret_val = %d.\n", ret_val);
        goto destroy_cdev;
	}
    __PRINTK(MAG,"Creating device %s.\n", &ssc_dev->dev_name[0]);  
    /* A "dev" file will be created, showing the dev_t for the device, if the dev_t is not 0,0. */
    tmp_dev = device_create (
                            ssc_dev->ssc_class,     /* pointer to the struct class that this device should be registered to */
                            NULL,                   /* pointer to the parent struct device of this new device */
                            ssc_dev->dev_num,       /* the 'dev_t' for the char device to be added */
                            (void *)ssc_dev,        /* the data to be added to the device for callbacks */
                            &ssc_dev->dev_name[0]   /* string for the device's name */
                            );
    if (IS_ERR(tmp_dev)) 
    {
		ret_val = PTR_ERR(tmp_dev);
        __PRINTK(RED,"ERROR : Can't create 'ssc' device. ret_val = %d.\n", ret_val);
        goto destroy_class;
	}
                  
    __PRINTK(GRN,   "Char device '/dev/%s' successfully initialized. Major : %i. Minor : %i.\n", 
                    ssc_dev->dev_name, MAJOR(ssc_dev->dev_num), MINOR(ssc_dev->dev_num));           
    return 0;
     
destroy_class:
    class_destroy(ssc_dev->ssc_class);
destroy_cdev:
    unregister_chrdev_region(ssc_dev->dev_num, SSC_DEVICES);
unregister_chrdev:
    unregister_chrdev_region(ssc_dev->dev_num, SSC_DEVICES);   
    return -1;
    
}
/*--- End of function ----------------------------------------------------------------------------*/

int ssc_destroy_dev_iface(struct ssc_device * ssc_dev)
{

    
    
    mutex_unlock(&ssc_dev->mutex);
    if (ssc_dev->ssc_class != NULL)
    {
        __PRINTK(RED, "Removing 'dev' interface.\n");
        device_destroy(ssc_dev->ssc_class, ssc_dev->dev_num);
        class_destroy(ssc_dev->ssc_class);
    }
    cdev_del(&ssc_dev->cdev);
    unregister_chrdev_region(ssc_dev->dev_num, SSC_DEVICES);

    free_page((unsigned long)ssc_dev->dma_buf);

    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

                    
