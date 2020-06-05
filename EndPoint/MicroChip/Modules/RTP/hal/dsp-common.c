/*
 * $Id: dsp-common.c,v 1.1 2006/08/31 15:11:16 root Exp $
 */

#include "driver.h"
#include "dsp-hal.h"

extern struct dsp dsp[];

static void dsp_set_ownername(struct dsp *d, char *name)
{
	strncpy(d->owner_name, name, DSP_OWNER_MAXNAMELEN);
	d->owner_name[DSP_OWNER_MAXNAMELEN] = 0;
}
/*- End of function --------------------------------------------------------*/
void dsp_set_poll_handler(dsp_t dsp, dsp_handler_t function, void *context)
{
	struct dsp *d = dsp_get(dsp);
	d->poll_context = context;
	d->dsp_poll_function = function;
	d->do_dsp_poll = NULL;
}
/*- End of function --------------------------------------------------------*/
void dsp_enable_poll(dsp_t dsp)
{
	struct dsp *d = dsp_get(dsp);
	d->do_dsp_poll = d->dsp_poll_function;
}
/*- End of function --------------------------------------------------------*/
void dsp_disable_poll(dsp_t dsp)
{
	struct dsp *d = dsp_get(dsp);
	d->do_dsp_poll = NULL;
}
/*- End of function --------------------------------------------------------*/
int dsp_allocate(int dnum, dsp_t * d, char *name)
{
	if (dnum != DSP_NUMBER_DONT_CARE) {
		if (dnum >= 0 && dnum <= last_dsp_number && dsp[dnum].present) {
			if (!dsp[dnum].busy) {
				dsp[dnum].busy = 1;
				*d = (dsp_t) (dsp + dnum);
				if (name) {
					dsp_set_ownername(dsp_get(*d), name);
				}
				return dnum;
			}
			return -EBUSY;
		}
		return -ENOENT;
	} else {
		u_int i;
		for (i = 0; i <= last_dsp_number; i++) {
			if (dsp[i].present) {
				if (!dsp[i].busy) {
					dsp[i].busy = 1;
					*d = (dsp_t) (dsp + i);
					if (name) {
						dsp_set_ownername(dsp_get(*d), name);
					}
					return i;
				}
			}
		}

		return -EBUSY;
	}
}
/*- End of function --------------------------------------------------------*/
void dsp_free(dsp_t dsp)
{
	struct dsp *d = dsp_get(dsp);

	if (d->busy) {

		d->owner_name[0] = '\0';
		dsp_free_tdm(dsp, -1);
		d->busy = 0;
	}
}
/*- End of function --------------------------------------------------------*/
dsp_t dsp_lookup(int number)
{
	if (number >= 0 && number <= last_dsp_number && dsp[number].present)
		return (dsp_t) (dsp + number);
	else
		return 0;
}
/*- End of function --------------------------------------------------------*/
int dsp_get_last_number(void) {return last_dsp_number;}
/*- End of function --------------------------------------------------------*/
int dsp_get_number(dsp_t dsp) {return dsp_get(dsp)->number;}
/*- End of function --------------------------------------------------------*/
const char *dsp_get_owner(dsp_t dsp) {return dsp_get(dsp)->owner_name;}
/*- End of function --------------------------------------------------------*/
static __inline__ int check_and_mark_chn(struct dsp_bus *bus, int *res, int chn, int remain, int strict)
{
	if (bus->ts_busy[chn] == -1) {
		*res = chn;
		--remain;
	} else {
		if (strict) return -EBUSY;
	}
	return remain;
}
/*- End of function --------------------------------------------------------*/
void dsp_init_buses(struct dsp_bus *buses, int count)
{
	int i, j;
	for (i = 0; i < count; i++) {
		sema_init(&buses[i].lock, 1);
		for (j = 0; j < CHNS_PER_BUS; j++)
			buses[i].ts_busy[j] = -1;
	}
}
/*- End of function --------------------------------------------------------*/
int dsp_bus_get_free_chns( int bus_number )
{
	int i, free = 0;
	struct dsp_bus* bus = dsp_get_bus( bus_number );

	for( i=0; i<CHNS_PER_BUS; ++i )
		if( bus->ts_busy[i] == -1 )
			++free;
	return free;
}
/*- End of function --------------------------------------------------------*/
/*
	if strict is set to non-zero value, only consecutive channels will be allocated,
	otherwise function will fails with -EBUSY error code

*/
int dsp_alloc_tdm(dsp_t dsp, int num_chns, int from_pref, int strict, int *chns)
{
	struct dsp* d = dsp_get(dsp);
	struct dsp_bus* bus = d->tdm_bus;
	int chns_rem, i, pos_from;

	chns_rem = num_chns;
	d->num_channels = num_chns;
	pos_from = from_pref < 0 ? -from_pref : from_pref;

	if ((strict && from_pref < 0) || num_chns <= 0)
		return -EINVAL;

	if (pos_from >= CHNS_PER_BUS) {
		if (strict)
			return -EINVAL;	/* channel number is to big */
		else
			pos_from = CHNS_PER_BUS - 1;
	}
	down(&bus->lock);
	if (from_pref < 0) {
		for (i = pos_from; i >= 0; i--) {
			chns_rem = check_and_mark_chn(bus, chns + num_chns - chns_rem, i, chns_rem, 0);
			if (chns_rem < 0)
				goto no_more_free_channels;
			if (!chns_rem)
				break;
		}
		if (chns_rem) {
			for (i = CHNS_PER_BUS - 1; i > pos_from; i--) {
				chns_rem = check_and_mark_chn(bus, chns + num_chns - chns_rem, i, chns_rem, 0);
				if (chns_rem < 0)
					goto no_more_free_channels;
				if (!chns_rem)
					break;
			}
		}
		if (chns_rem)
			goto no_more_free_channels;
	} else {
		for (i = pos_from; i < CHNS_PER_BUS; i++) {
			chns_rem = check_and_mark_chn(bus, chns + num_chns - chns_rem, i, chns_rem, strict);
			if (chns_rem < 0)
				goto no_more_free_channels;
			if (!chns_rem)
				break;
		}
		if (chns_rem) {
			for (i = 0; i < pos_from - 1; i++) {
				chns_rem = check_and_mark_chn(bus, chns + num_chns - chns_rem, i, chns_rem, strict);
				if (chns_rem < 0)
					goto no_more_free_channels;
				if (!chns_rem)
					break;
			}
		}
		if (chns_rem)
			goto no_more_free_channels;
	}

	if (from_pref < 0)
		d->tdm_offset = chns[num_chns-1];	/* modem case */
	else
		d->tdm_offset = chns[0];			/* voice */
		
	d->tdm_base = d->tdm_offset + (d->bus_number * CHNS_PER_BUS);
	for (i = 0; i < num_chns; i++)
	{
//		printk("dsp[%d:%d] timeslot = %d\n", d->number,i, chns[i]);
		bus->ts_busy[chns[i]] = d->number;
	}
	up(&bus->lock);
	return 0;

no_more_free_channels:
	up(&bus->lock);

	return -EBUSY;
}
/*- End of function --------------------------------------------------------*/
void dsp_free_tdm(dsp_t dsp, int chn)
{
	struct dsp *d;
	struct dsp_bus *bus;

	int i;

	d = dsp_get(dsp);
	bus = d->tdm_bus;

	if (chn >= CHNS_PER_BUS || (chn < 0 && chn != -1)) return;
	down(&bus->lock);

	if (chn < 0) {
		for (i = 0; i < CHNS_PER_BUS; i++) {
			if (bus->ts_busy[i] == d->number) {
				bus->ts_busy[i] = -1;
			}
		}
	} else {
		if (bus->ts_busy[chn] == d->number) {
			bus->ts_busy[chn] = -1;
		}
	}
	d->tdm_base = -1;
	d->num_channels = 0;
	up(&bus->lock);
}
/*- End of function --------------------------------------------------------*/
EXPORT_SYMBOL(dsp_get_last_number);
EXPORT_SYMBOL(dsp_get_number);
EXPORT_SYMBOL(dsp_allocate);
EXPORT_SYMBOL(dsp_alloc_tdm);
EXPORT_SYMBOL(dsp_free);
