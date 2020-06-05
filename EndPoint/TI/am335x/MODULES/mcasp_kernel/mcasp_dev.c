#include "mcasp_mod.h"

/*! Signature :
 * ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
 */
__weak ssize_t mcasp_read	(
							struct file *	filp,
							char __user *	buf,
							size_t			count,
							loff_t *		f_pos
							)
{
	ssize_t			 		ret_val		= 0;
	struct mcasp_device *	mcasp_dev	= filp->private_data;
	char 					msg[]		= "Read\r\n";
	__PRINTK(CYN, "Reading device %s.\n", mcasp_dev->plat_data->dev_name);
	if (mutex_lock_interruptible(&mcasp_dev->mutex)) { return -ERESTARTSYS; }
		count = sizeof(msg);
		if (copy_to_user(buf, msg, count))
		{
			ret_val = -EFAULT;
			goto out;
		}
		*f_pos += count;
		ret_val = 0; /* If the value is 0, end-of-file was reached (and no data was read). */
out:
	mutex_unlock(&mcasp_dev->mutex);
	return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

__weak ssize_t mcasp_write	(
							struct file *		filp,
							const char __user *	buf,
							size_t				count,
							loff_t *			f_pos
							)
{
	struct mcasp_device *	mcasp_dev	= filp->private_data;
	ssize_t					ret_val		= 0;
	__PRINTK(CYN, "Writing device %s.\n", mcasp_dev->plat_data->dev_name);
	*f_pos += count;
	ret_val = count;
	return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

int mcasp_open (struct inode * inode_ptr, struct file * filp)
{
	struct mcasp_device * mcasp_dev = container_of(inode_ptr->i_cdev, struct mcasp_device, cdev);
	filp->private_data = mcasp_dev; /* for other methods */
	__PRINTK(CYN, "Opening device %s.\n", mcasp_dev->plat_data->dev_name);
	return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

int mcasp_release (struct inode * inode_ptr, struct file * filp)
{
	struct mcasp_device * mcasp_dev = container_of(inode_ptr->i_cdev, struct mcasp_device, cdev);
	__PRINTK(CYN, "Releasing device %s.\n", mcasp_dev->plat_data->dev_name);
	return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/

static const struct file_operations	mcasp_dev_fops = {
	.owner		= THIS_MODULE,
	.read		= mcasp_read,
	.write		= mcasp_write,
	.open		= mcasp_open,
	.release	= mcasp_release,
};

#define McASP_FIRST_MINOR 	0
#define McASP_DEVICES	 	1


int mcasp_create_dev_iface(struct mcasp_device * mcasp_dev)
{
	int ret_val = 0;
	struct device * tmp_dev;

	 __PRINTK(YEL,"Creating 'dev' interface for '%s' controller.\n", mcasp_dev->plat_data->dev_name);

	__PRINTK(MAG,"Allocating driver numbers.\n");
#if (1)
	ret_val = alloc_chrdev_region(&mcasp_dev->dev_num, McASP_FIRST_MINOR, McASP_DEVICES, &mcasp_dev->plat_data->dev_name[0]);
#else
	mcasp_dev->dev_num = 200;
	ret_val = register_chrdev_region(
									mcasp_dev->dev_num,				/* output-only parameter that will, on successful completion,	hold	the	first	number	in	your	allocated	range */
									McASP_DEVICES,					/* total number of contiguous device numbers you are requesting */
									mcasp_dev->plat_data->dev_name	/* name of the device that should be associatedwith this number range */
									);
#endif
	if (ret_val < 0)
	{
		__PRINTK(RED, "Can't allocate number for controller %s.\n", mcasp_dev->plat_data->dev_name);
		return ret_val;
	}
	mutex_init(&mcasp_dev->mutex);

	__PRINTK(MAG,"Creating char device.\n");
	cdev_init(&mcasp_dev->cdev, &mcasp_dev_fops);
	mcasp_dev->cdev.owner = THIS_MODULE;
	ret_val = cdev_add	(
						&mcasp_dev->cdev,	 /* struct cdev *	*/
						mcasp_dev->dev_num,	/* the first device number for which this device is responsible */
						McASP_DEVICES		/* the number of consecutive minor numbers corresponding to this device */
						);
	if (ret_val)
	{
		__PRINTK(RED,	"ERROR : Can't add cdev for '%s' controller. ret_val = %d.\n",
						mcasp_dev->plat_data->dev_name, ret_val);
		goto unregister_chrdev;
	}


	__PRINTK(MAG,"Creating class.\n");
	mcasp_dev->mcasp_class = class_create(THIS_MODULE, mcasp_dev->plat_data->dev_name);
	if (IS_ERR(mcasp_dev->mcasp_class)) {
		ret_val = PTR_ERR(mcasp_dev->mcasp_class);
		__PRINTK(RED,"ERROR : Can't create 'mcasp' class. ret_val = %d.\n", ret_val);
		goto delete_cdev;
	}
	__PRINTK(MAG,"Creating device %s.\n", mcasp_dev->plat_data->dev_name);
	/* A "dev" file will be created, showing the dev_t for the device, if the dev_t is not 0,0. */
	tmp_dev = device_create (
							mcasp_dev->mcasp_class,			 /* pointer to the struct class that this device should be registered to */
							NULL,							/* pointer to the parent struct device of this new device */
							mcasp_dev->dev_num,				/* the 'dev_t' for the char device to be added */
							(void *)mcasp_dev,				/* the data to be added to the device for callbacks */
							mcasp_dev->plat_data->dev_name	/* string for the device's name */
							);
	if (IS_ERR(tmp_dev))
	{
		ret_val = PTR_ERR(tmp_dev);
		__PRINTK(RED,	"ERROR : Can't create '%s' device. ret_val = %d.\n",
						mcasp_dev->plat_data->dev_name, ret_val);
		goto destroy_class;
	}

	__PRINTK(GRN,	"Char device '/dev/%s' successfully initialized. Major : %i. Minor : %i.\n",
					mcasp_dev->plat_data->dev_name, MAJOR(mcasp_dev->dev_num), MINOR(mcasp_dev->dev_num));

	return 0;

destroy_class:
	class_destroy(mcasp_dev->mcasp_class);
delete_cdev:
	cdev_del(&mcasp_dev->cdev);
unregister_chrdev:
	unregister_chrdev_region(mcasp_dev->dev_num, McASP_DEVICES);
	return -1;
}
/*--- End of function ----------------------------------------------------------------------------*/

int mcasp_destroy_dev_iface(struct mcasp_device * mcasp_dev)
{
	__PRINTK(RED, "Removing 'dev' interface.\n");
	mutex_unlock(&mcasp_dev->mutex);
	if (mcasp_dev->mcasp_class != NULL)
	{
		__PRINTK(RED,"ERROR : NULL passed as the parameter.\n");
		device_destroy(mcasp_dev->mcasp_class, mcasp_dev->dev_num);
		class_destroy(mcasp_dev->mcasp_class);
	}
	cdev_del(&mcasp_dev->cdev);
	unregister_chrdev_region(mcasp_dev->dev_num, McASP_DEVICES);

	return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/


