#include "driver.h"
#include "hal.h"
#include "hal-ioctl.h"

int hal_ioctl_flash_setup       (struct file *file)
{
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_flash_unsetup     (struct file *file)
{
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_flash_write       (struct file *file, struct param_flash_write *ios)
{
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_flash_read        (struct file *file, struct param_flash_read *ios)
{
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/
