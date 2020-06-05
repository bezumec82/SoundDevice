#include "driver.h"
#include "hal.h"
#include "hal-ioctl.h"

extern int max_number_of_dsp;

int hal_ioctl_read_dsp(struct file *file, struct param_read_dsp *parm)
{
    int error;
    dsp_t dsp;
    if (parm->number >= max_number_of_dsp)
    {
        error = -EINVAL;
        goto bail_out;
    }

    dsp = dsp_lookup(parm->number);
    if (!dsp)
    {
        error = -ENOENT;
        goto bail_out;
    }

    parm->data = 0x00;

    error = 0;
bail_out:
    return error;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_write_dsp(struct file *file, struct param_write_dsp *parm)
{
    int error;
    dsp_t dsp;

    if (parm->number >= max_number_of_dsp)
    {
        error = -EINVAL;
        goto bail_out;
    }

    dsp = dsp_lookup(parm->number);
    if (!dsp)
    {
        error = -ENOENT;
        goto bail_out;
    }

    error = 0;
bail_out:
    return error;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_reset_dsp(struct file *file, struct param_reset_dsp *parm)
{
    int error;
    dsp_t dsp;

    if (parm->number >= max_number_of_dsp)
    {
        error = -EINVAL;
        goto bail_out;
    }

    dsp = dsp_lookup(parm->number);
    if (!dsp)
    {
        error = -ENOENT;
        goto bail_out;
    }

    if (parm->action == RESETDSP_ACTION_RESET)
    {
        dsp_reset(dsp);
    }
    else if (parm->action == RESETDSP_ACTION_UNRESET)
    {
        dsp_unreset(dsp);
    }
    else
    {
        error = -EINVAL;
        goto bail_out;
    }

    error = 0;
bail_out:
    return error;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_dsp_owner(struct file *file, struct param_dsp_owner *parm)
{
    int error;
    dsp_t dsp;

    if (parm->number >= max_number_of_dsp)
    {
        error = -EINVAL;
        goto bail_out;
    }

    dsp = dsp_lookup(parm->number);
    if (!dsp)
    {
        error = -ENOENT;
        goto bail_out;
    }

    strncpy(parm->owner_name, dsp_get_owner(dsp), DSP_OWNER_MAXNAMELEN);
    parm->bus = dsp->bus_number;
    parm->ts_base = dsp->tdm_base;
    parm->ts_count = dsp->num_channels;
    parm->owner_name[DSP_OWNER_MAXNAMELEN] = 0;

    error = 0;
bail_out:
    return error;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_dsp_get_max(struct file *file, struct param_dsp_get_max *parm)
{
    int error=0;

    parm->max = max_number_of_dsp;

    return error;
}
/*- End of function ------------------------------------------------------------------------------*/

int hal_ioctl_dsp_upload(struct param_dsp_upload *ios)
{
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/
