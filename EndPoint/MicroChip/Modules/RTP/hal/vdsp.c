/*
 * $Id: dsp-20-30.c,v 1.1 2006/08/31 15:11:16 root Exp $
 */

#include "driver.h"
#include "dsp-hal.h"

#define MAX_DSP_MODULES               1  /* 8 */
#define NUM_DSP_PER_MODULE            4  /* 4 */
#define NUM_DSP_PER_MOD_MODULE        16 /* 16 */

// why the hell MAX_DSP_MODULES*16?
#define MAX_DSP                       (MAX_DSP_MODULES * 16)

#define DSP_FIRST_BUS                 0 /* 8 */
#define DSP_LAST_BUS                  0 /* 15 */

static struct dsp_bus dsp_buses[DSP_LAST_BUS - DSP_FIRST_BUS + 1];

static struct module_desc modules[MAX_DSP_MODULES] = {
	[0] = {.bus=0}
};

struct dsp dsp[MAX_DSP];

int max_number_of_dsp = MAX_DSP;
int last_dsp_number = -1;
int number_of_dsp = 0;
int poll_enabled = 0;

static int dsp_init_module(int mod_number, int index);

/*--------------------------------------------------------------------------*/
int dsp_get_first_bus()
{
	return DSP_FIRST_BUS;
};
/*- End of function --------------------------------------------------------*/
int dsp_get_last_bus()
{
	return DSP_LAST_BUS;
};
/*- End of function --------------------------------------------------------*/
struct dsp_bus* dsp_get_bus( int bus_number )
{
	return &dsp_buses[bus_number-DSP_FIRST_BUS];
}
/*- End of function --------------------------------------------------------*/
int dsp_init(void *context)
{
	int index = 0, i;

	dsp_init_buses(dsp_buses, sizeof(dsp_buses) / sizeof(struct dsp_bus));

	for (i = 0; i < MAX_DSP_MODULES; i++) {
		int count, j, working;
		count = dsp_init_module(i, index);

		for (j = 0, working = 0; j < count; j++) {
			if (dsp_base_check(dsp + index + j) >= 0) {
				dsp[index + j].present = 1;
				number_of_dsp++;
				working++;
				last_dsp_number = index + j;
			}
			dsp_reset((dsp_t) (dsp + index + j));
		}

		if (working) {
			modules[i].present = 1;
			modules[i].irq_ctx = dsp + index;
			index += count;
		}
	}
	if (number_of_dsp)
		PRINTK(KERN_INFO, "found %d dsp\n", number_of_dsp);
	else
		PRINTK(KERN_INFO, "no dsp found\n");

	poll_enabled = 1;
	return 0;
}
/*- End of function --------------------------------------------------------*/
void dsp_deinit(void *context)
{
}
/*- End of function --------------------------------------------------------*/
static int dsp_init_module(int mod_number, int index)
{
	int count, i;
	
	count = NUM_DSP_PER_MODULE;
	for (i = 0; i < count; i++) {
		struct dsp *d = dsp + index + i;

		d->bus_number = modules[mod_number].bus;
	}

	for (i = 0; i < count; i++) {
		struct dsp *d = dsp + index + i;

		d->tdm_bus = &dsp_buses[d->bus_number - DSP_FIRST_BUS];
		d->number = index + i;
		d->n_dsp_in_module = count;

		d->mod_number = mod_number + 1;
		d->offset_in_mod = i;
		d->do_dsp_poll = NULL;
		d->dsp_poll_function = NULL;

		d->owner_name[0] = '\0';
	}

	return count;
}
/*- End of function --------------------------------------------------------*/
void dsp_poll()
{
	int i, n, f;
	struct dsp *d;

	for (i = 0; i < MAX_DSP_MODULES; i++) {
		if (modules[i].present) {
			d = (struct dsp *) modules[i].irq_ctx;

			n = d->n_dsp_in_module;
			for (f = 0; f < n; ++f, ++d) {

				if (d->do_dsp_poll)
					d->do_dsp_poll(d->poll_context);
			}			
		}
	}
}
/*- End of function --------------------------------------------------------*/
void dsp_reset(dsp_t dsp)
{
}
/*- End of function --------------------------------------------------------*/
void dsp_unreset(dsp_t dsp)
{
}
/*- End of function --------------------------------------------------------*/
void dsp_enable_interrupt(dsp_t dsp)
{
}
/*- End of function --------------------------------------------------------*/
void dsp_disable_interrupt(dsp_t dsp)
{
}
/*- End of function --------------------------------------------------------*/
void dsp_check_reset_cache(void)
{
}
/*- End of function --------------------------------------------------------*/
