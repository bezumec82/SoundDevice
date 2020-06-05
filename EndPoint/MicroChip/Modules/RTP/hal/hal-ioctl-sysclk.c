#include "driver.h"
#include "hal.h"
#include "hal-ioctl.h"

/*! Wrapper for #get_sysclk_state call. Not implemented. */
int hal_ioctl_get_sysclk_state (struct file *file, struct param_get_sysclk_state *parm)
{
    int error=0;
/*
    parm->prognosed_state = get_sysclk_state(&parm->freq, &parm->freq_frac);
*/
    return error;
}

/*! Wrapper for #clock_select call. Not implemented. */
int hal_ioctl_set_clock (struct file *file, struct param_set_clock *parm)
{
    int error=0;
/*
    clock_select(parm->clock);
*/
    return error;
}

/*! Wrapper for #get_clock_source call. Not implemented. */
int hal_ioctl_get_clock(struct file *file, struct param_get_clock *parm)
{
    int error=0;
/*
    parm->clock = get_clock_source();
*/
    return error;
}
/*- End of function ------------------------------------------------------------------------------*/
