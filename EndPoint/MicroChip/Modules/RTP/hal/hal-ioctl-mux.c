#include "driver.h"
#include "hal.h"
#include "hal-ioctl.h"

int hal_ioctl_read_mux              (struct file *file, struct param_read_mux *parm)
{
    int error;

    switch (parm->address & 0xFF000000)
    {
    case MUX_ADDR_CFG:
        parm->data = 0x00;
        break;
    case MUX_ADDR_CM_BACKPLANE:
        parm->data = 0x00;
        break;
    case MUX_ADDR_CM_LOCAL_LOW:
        parm->data = 0x00;
        break;
    case MUX_ADDR_CM_LOCAL_HIGH:
        parm->data = 0x00;
        break;
    case MUX_ADDR_DM_BACKPLANE:
        parm->data = 0x00;
        break;
    case MUX_ADDR_DM_LOCAL:
        parm->data = 0x00;
        break;
    default:
        error = -EINVAL;
        goto bail_out;
    }

    error = 0;
      bail_out:
    return error;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_write_mux             (struct file *file, struct param_write_mux *parm)
{
    int error;

    switch (parm->address & 0xFF000000)
    {
    case MUX_ADDR_CFG:
        break;
    case MUX_ADDR_CM_BACKPLANE:
        break;
    case MUX_ADDR_CM_LOCAL_LOW:
        break;
    case MUX_ADDR_CM_LOCAL_HIGH:
        break;
    case MUX_ADDR_DM_BACKPLANE:
    case MUX_ADDR_DM_LOCAL:
        error = -EACCES;
        goto bail_out;
    default:
        error = -EINVAL;
        goto bail_out;
    }

    error = 0;
      bail_out:
    return error;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_mux_connect           (struct file *file, struct param_connect *parm)
{
    int error=0;

    mux_connect(parm->destination, parm->source);

    return error;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_mux_disconnect        (struct file *file, struct param_disconnect *parm)
{
    int error=0;

    mux_disconnect(parm->channel);

    return error;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_write_mux_msg         (struct file *file, struct mux_msg_write *ios)
{
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_mux_on_fatal_error    (struct file *file, int action)
{
    switch( action )
    {
        case HAL_FATAL_ERROR_IGNORE:
            mux_set_kernel_panic_enabled( 0 );
            break;

        case HAL_FATAL_ERROR_KERNEL_PANIC:
            mux_set_kernel_panic_enabled( 1 );
            break;

        default:
            return -EINVAL;
    }
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_read_mux_msg          (struct file *file, struct mux_msg_read *ios)
{
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/
