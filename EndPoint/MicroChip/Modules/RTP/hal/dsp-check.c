
#include "driver.h"
#include "dsp-hal.h"

const char vdsp[] = "Virtual DSP";
/*--------------------------------------------------------------------------*/
int dsp_base_check(struct dsp *d)
{
	d->tdm_base = -1;
	d->dsp_type_str = vdsp;
	PRINTK(KERN_DEBUG, "UP%u_dsp%u: %s (%u)\n", d->mod_number, d->offset_in_mod, vdsp, d->number);
	dsp_reset((dsp_t) d);
	return 0;
}
/*- End of function --------------------------------------------------------*/
