/*
 * $Id: dsp-hal.h,v 1.1 2006/08/31 15:11:16 root Exp $
 */

#ifndef __DSP_HAL_H
#define __DSP_HAL_H

#define CHNS_PER_BUS			32

typedef void (*dsp_handler_t) (void *);

extern int poll_enabled;
void dsp_poll(void);

struct dsp_bus {
	u_int number;
	struct semaphore lock;
	int ts_busy[CHNS_PER_BUS];
};

struct module_desc {
	/* static information */
	int present;
	int bus;
	/* dynamic information */
	struct dsp *irq_ctx;
};

struct dsp {
	int present;
	int busy;
	int number;
	int n_dsp_in_module;
	int type;
	int offset_in_mod;
	int mod_number;

	/* hardware related information */
	void *poll_context;
	dsp_handler_t dsp_poll_function;
	dsp_handler_t do_dsp_poll;

	char owner_name[DSP_OWNER_MAXNAMELEN + 1];
	const char *dsp_type_str;

	int tdm_offset;
	int tdm_base;
	u_int num_channels;

	int bus_number;
	struct dsp_bus *tdm_bus;
};

typedef struct dsp * dsp_t;
extern int last_dsp_number;
extern int number_of_dsp;

extern int dsp_base_check(struct dsp *d);

static inline struct dsp *dsp_get(dsp_t dsp) {	return (struct dsp*)dsp;}
static inline void dsp_put(dsp_t dsp) {}
static inline int dsp_tdm_base(dsp_t dsp) { return dsp->tdm_base;}

void dsp_init_buses(struct dsp_bus *buses, int count);
int dsp_bus_get_free_chns( int bus_number );
int dsp_get_first_bus( void );
int dsp_get_last_bus( void );
struct dsp_bus* dsp_get_bus( int bus_number );

#endif
