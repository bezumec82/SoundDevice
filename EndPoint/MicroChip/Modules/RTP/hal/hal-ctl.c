/*! @file hal-ctl.c
 * Hal char driver implementation.
 */

#include "driver.h"
#include "hal.h"
#include "hal-ioctl.h"
#include "vmux.h"


#define HAL_CLASSNAME               "kvoice_hal"

static struct file_operations hal_fops;
extern int              max_number_of_dsp;
static int              hal_major   = 0;
static struct class*    hal_class   = NULL;
static struct device*   hal_device  = NULL;

/*- End of function ------------------------------------------------------------------------------*/

/*! Creates /dev/hal char driver,
 * that controls hal module, and /sys/class/kvoice_hal.
 * @param context - Not used in this routine.
 *                  Structure #hal_context will be passed by #init_startup.
 */
int hal_ctl_init(void * context)
{
    __PRINTK(GRN, "Initializing HAL contorl.\n"); 
    hal_major = register_chrdev(0, HAL_DEVNAME, &hal_fops);
    if (hal_major < 0)
    {
        printk(KERN_DEBUG  "failed to register device \"" HAL_DEVNAME  "\" with error %d\n", hal_major);
        return -EBUSY;
    }
    
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    hal_class = class_create(THIS_MODULE, HAL_CLASSNAME);
    if (IS_ERR(hal_class))
    {
        unregister_chrdev(hal_major, HAL_DEVNAME);
        printk(KERN_ALERT "Failed to register device class '%s'\n", HAL_CLASSNAME);
        return PTR_ERR(hal_class);
    }

    hal_device = device_create(hal_class, NULL, MKDEV(hal_major, 0), NULL, HAL_DEVNAME);
    if (IS_ERR(hal_device))
    {
        class_destroy(hal_class);
        unregister_chrdev(hal_major, HAL_DEVNAME);
        printk(KERN_ALERT "Failed to create the device '%s'\n", HAL_DEVNAME);
        
        return PTR_ERR(hal_device);
    }
#endif
    __PRINTK(GRN, "Device '/hal/dev' succesfully created.\n"); 
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/

/*! De-initializes /dev/hal driver. 
 * @param context - Not used in this routine.
 *                  Structure #hal_context will be passed by #init_startup.
 */
void hal_ctl_deinit(void *context)
{
    __PRINTK(GRN, "De-initializing HAL contorl.\n"); 
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    device_destroy(hal_class, MKDEV(hal_major, 0));
    class_unregister(hal_class);
    class_destroy(hal_class);
#endif
    unregister_chrdev(hal_major, HAL_DEVNAME);
}
/*- End of function ------------------------------------------------------------------------------*/

/*! /dev/hal open call. */
static int hal_open(struct inode * inode, struct file * file)
{
    struct hal_fd_context *fd_ctx;

    module_inc_use_count();

    file->private_data = kmalloc(sizeof(struct hal_fd_context), GFP_KERNEL);
    if (!file->private_data)
    {
        module_dec_use_count();
        return -ENOMEM;
    }

    fd_ctx = (struct hal_fd_context *) file->private_data;
    memset(fd_ctx, 0, sizeof(struct hal_fd_context));
    fd_ctx->flash.par_id = -1;
    sema_init(&fd_ctx->flash.lock, 1);
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/

/*! /dev/hal release call. */
static int hal_release(struct inode *inode, struct file *file)
{
    struct hal_fd_context *fd_ctx;
    int i;

    fd_ctx = (struct hal_fd_context *) file->private_data;

    if (fd_ctx->lamp_sema_info < 0)
    {
        for (i = 0; i > fd_ctx->lamp_sema_info; i--)
        {
            /*lamp_sema_up();*/
        }
    }
    else
    {
        for (i = 0; i < fd_ctx->lamp_sema_info; i++)
        {
            /*lamp_sema_down();*/
        }
    }
    kfree(fd_ctx);
    module_dec_use_count();
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/

/*! /dev/hal poll call. Not implemented.
 */
static u_int hal_poll(struct file *file, struct poll_table_struct *pt)
{
    int poll_status;

    poll_status = 0;
/*
    if (!get_sysclk_change())
    {
        poll_wait(file, &hal_poll_wq, pt);
        if (signal_pending(current))
        {
            poll_status = -ERESTARTSYS;
            goto out;
        }
    }
    if (get_sysclk_change())
    {
        poll_status = POLLIN;
    }
    out:*/
    return poll_status;
}
/*- End of function ------------------------------------------------------------------------------*/

/*! HAL ioctl calls implementation.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int hal_ioctl(struct inode *inode, struct file *file, u_int cmd, u_long arg)
#else
static long hal_ioctl(struct file *file, u_int cmd, u_long arg)
#endif
{
    int error;
    __PRINTK(KERN_ERR, "hal_ioctl(cmd=0x%02X)\n", cmd);
    switch (cmd)
    {
        /*! -# TDM ioctls */
        case HAL_TDM_FIRST_BUS:     error = hal_ioctl_get_tdm_first_bus (file, (struct param_tdm_bus_number*) arg);     break;
        case HAL_TDM_LAST_BUS:      error = hal_ioctl_get_tdm_last_bus  (file, (struct param_tdm_bus_number*) arg);     break;
        case HAL_TDM_BUS_INFO:      error = hal_ioctl_get_tdm_bus_info  (file, (struct param_tdm_bus_info*) arg);       break;        
        /*! -# Sys cloc ioctls */
        case HAL_GET_CLOCK:         error = hal_ioctl_get_clock         (file, (struct param_get_clock *) arg);         break;
        case HAL_SET_CLOCK:         error = hal_ioctl_set_clock         (file, (struct param_set_clock *) arg);         break;
        case HAL_GET_SYS_CLK_STATE: error = hal_ioctl_get_sysclk_state  (file, (struct param_get_sysclk_state *) arg);  break;            
        /*! -# DSP ioctls */
        case HAL_READ_DSP:          error = hal_ioctl_read_dsp          (file, (struct param_read_dsp *) arg);          break;
        case HAL_WRITE_DSP:         error = hal_ioctl_write_dsp         (file, (struct param_write_dsp *) arg);         break;
        case HAL_RESET_DSP:         error = hal_ioctl_reset_dsp         (file, (struct param_reset_dsp *) arg);         break;
        case HAL_DSP_OWNER:         error = hal_ioctl_dsp_owner         (file, (struct param_dsp_owner *) arg);         break;
        case HAL_DSP_GET_MAX:       error = hal_ioctl_dsp_get_max       (file, (struct param_dsp_get_max *) arg);       break;
        case HAL_DSP_UPLOAD:        error = hal_ioctl_dsp_upload        ((struct param_dsp_upload *) arg);              break;        
        /*! -# MUX ioctls */
        case HAL_READ_MUX:          error = hal_ioctl_read_mux          (file, (struct param_read_mux *) arg);          break;
        case HAL_WRITE_MUX:         error = hal_ioctl_write_mux         (file, (struct param_write_mux *) arg);         break;    
        case HAL_MUX_CONNECT:       error = hal_ioctl_mux_connect       (file, (struct param_connect *) arg);           break;
        case HAL_MUX_DISCONNECT:    error = hal_ioctl_mux_disconnect    (file, (struct param_disconnect *) arg);        break;    
        case HAL_MUX_MSG_WR:        error = hal_ioctl_write_mux_msg     (file, (struct mux_msg_write *)arg);            break;    
        case HAL_MUX_MSG_RD:        error = hal_ioctl_read_mux_msg      (file, (struct mux_msg_read *)arg);             break;    
        case HAL_MUX_READ_CM:       error = hal_ioctl_read_cmdm         (file, (struct param_read_mux *)arg, 1);        break;    
        case HAL_MUX_READ_DM:       error = hal_ioctl_read_cmdm         (file, (struct param_read_mux *)arg, 0);        break;    
        case HAL_MUX_ON_FATAL_ERROR:error = hal_ioctl_mux_on_fatal_error(file, (int)arg);                               break;   
        /*! -# Miscelaneous ioctls */
        case HAL_MAP_MMCR:          error = hal_ioctl_map_mmcr          (file, (union param_map_mmcr *) arg);           break;        
        case HAL_LAMP_SEMA_UP:      error = hal_ioctl_lamp_sema_up      (file);                                         break;
        case HAL_LAMP_SEMA_DOWN:    error = hal_ioctl_lamp_sema_down    (file);                                         break;
        /*! -# Flash ioctls */
        case HAL_FLASH_SETUP:       error = hal_ioctl_flash_setup       (file);                                         break;
        case HAL_FLASH_UNSETUP:     error = hal_ioctl_flash_unsetup     (file);                                         break;
        case HAL_FLASH_WRITE:       error = hal_ioctl_flash_write       (file, (struct param_flash_write *) arg);       break;
        case HAL_FLASH_READ:        error = hal_ioctl_flash_read        (file, (struct param_flash_read *) arg);        break;    
        /*! -# Memory ioctls */
        case HAL_READ_MEM:          error = hal_ioctl_read_mem          (file, (struct param_mem *) arg);               break;    
        case HAL_WRITE_MEM:         error = hal_ioctl_write_mem         (file, (struct param_mem *) arg);               break;
        
        /*! -# SPI ioctls */
        case HAL_READ_SPI:          error = 0;            break;
        case HAL_WRITE_SPI:         error = 0;            break;
        default:                    error = -EINVAL;
    } //end switch (cmd)
    return error;
}

/// HAL file operations structure.
static struct file_operations hal_fops = {
      open          : hal_open,
      release       : hal_release,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
      ioctl         : hal_ioctl,
#else
      unlocked_ioctl: hal_ioctl,
      compat_ioctl  : hal_ioctl,
#endif
      poll          : hal_poll,
};
/*- End of file ----------------------------------------------------------------------------------*/
