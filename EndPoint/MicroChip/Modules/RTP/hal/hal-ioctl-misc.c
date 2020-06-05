/*! @file hal-ioctl-misc.c
 * Several dead ends.
 */

#include "driver.h"
#include "hal.h"
#include "hal-ioctl.h"


struct flash_context;
struct hal_fd_context;


int hal_ioctl_map_mmcr          (struct file *file, union param_map_mmcr *parm)
{
    return -EINVAL;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_lamp_sema_up      (struct file *file)
{
    struct hal_fd_context *fd_ctx;

    fd_ctx = (struct hal_fd_context *) file->private_data;

    fd_ctx->lamp_sema_info++;

    /*lamp_sema_up();*/

    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/
int hal_ioctl_lamp_sema_down    (struct file *file)
{
    struct hal_fd_context *fd_ctx;

    fd_ctx = (struct hal_fd_context *) file->private_data;

    fd_ctx->lamp_sema_info--;

    /*lamp_sema_down();*/

    return 0;
}

int hal_ioctl_read_cmdm         (struct file *file, struct param_read_mux *ios, int do_read_cm)
{
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/

