/*
 * $Id: libparam.h,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#ifndef __LIBPARAM_H
#define __LIBPARAM_H

#define MAX_OPTIONS 8

enum {
	option_int = 1,
	option_string,
	option_set_flag,
	option_reset_flag,
	option_set_var,
};

struct option_desc {
	int type;
	char *name;
	void *dest;
	void *val;
};

enum {
	param_func,
	param_opt,
	param_func_always,
	param_func_prio,
};

#define DEFAULT_PARAM (1 << 16)
#define PARAM_TYPE_MASK ~DEFAULT_PARAM

typedef int(*param_func_t)(void *) ;

struct param {
	int type;
	char short_option;
	char *long_option;
	int mandatory_options;
	void *data;
	void *context;

	struct option_desc options[MAX_OPTIONS];

	int id;
	int chosen;
	int nparams;
	int long_sel;
	int n_cl_opts;
};

int parse_params(int argc, char **argv, struct param *params, int params_count);

#endif
