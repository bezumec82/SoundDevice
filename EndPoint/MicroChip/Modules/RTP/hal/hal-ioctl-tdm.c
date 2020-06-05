#include "driver.h"
#include "hal.h"
#include "hal-ioctl.h"

int hal_ioctl_get_tdm_first_bus (struct file *file, struct param_tdm_bus_number *parm)
{
    parm->value = dsp_get_first_bus();
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_get_tdm_last_bus  (struct file *file, struct param_tdm_bus_number *parm)
{
    parm->value = dsp_get_last_bus();
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/
int hal_ioctl_get_tdm_bus_info  (struct file *file, struct param_tdm_bus_info *parm)
{
    parm->channels = CHNS_PER_BUS;
    parm->free_channels = dsp_bus_get_free_chns( parm->bus_number );
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/
