/*
 * $Id: param.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include "libparam.h"
#include "libcommon.h"

#define START_LONG_OPT_ID 256

static struct param *app_params;
static int app_params_count;

static struct option *long_opts = NULL;
static char *short_opts = "";
static int short_opts_map[256];
static int *long_opts_map;
static struct param *default_param = NULL;
static	int long_opt_count = 0;
static	int short_opt_count = 0;
static	int short_opt_with_parm = 0;

int prepare_opts(void)
{
 int i, j, error, index, error_default;

	for (i = 0, error = 0, error_default = 0; i < app_params_count; i++) {
		if ((app_params[i].type & DEFAULT_PARAM) != 0) {
			if (default_param) {
				if (!error_default) {
 				fprintf(stderr, "internal error: more than one default parm specified\n");
 				error_default = 1;
 			}
				error = 1;
			} else {
				default_param = app_params + i;
			}
		}

		if (!app_params[i].short_option && !app_params[i].long_option) {
			fprintf(stderr, "internal error: no options (neither short nor long) specified in parm %d\n", i);
			error = 1;
		}
		if (app_params[i].short_option) {
			if (app_params[i].mandatory_options) {
				short_opt_with_parm++;
			}
		 short_opt_count++;
		}
		if (app_params[i].long_option) long_opt_count++;
		for (j = 0; j < MAX_OPTIONS; j++) {
			int stop = 0;
			switch (app_params[i].options[j].type) {
				case option_int:
				case option_string:
					app_params[i].n_cl_opts++;
				 break;
				default:
					stop = 1;
				 break;
			}
			if (stop) break;
		}

		if (j < app_params[i].mandatory_options) {
			fprintf(stderr, "internal error: invalid mandatory option count specified in parm %d\n", i);
			error = 1;
		}
	}

	if (error) return 1;

	if (long_opt_count) {
		long_opts = xmalloc(sizeof(struct option) * (long_opt_count + 1));
		long_opts_map = xmalloc(sizeof(int) * long_opt_count);
		for (i = 0, index = 0; i < app_params_count; i++) {
			if (app_params[i].long_option) {
				long_opts_map[index] = i;
				app_params[i].id = index + START_LONG_OPT_ID;

				long_opts[index].name = app_params[i].long_option;
				long_opts[index].has_arg = app_params[i].mandatory_options != 0;
				long_opts[index].val = app_params[i].id;

				index++;
			}
		}
	}
	if (short_opt_count) {
		short_opts = xmalloc(short_opt_count + short_opt_with_parm + 1);
		for (i = 0, index = 0; i < app_params_count; i++) {
			if (app_params[i].short_option > 0) {
				short_opts_map[app_params[i].short_option] = i;
				short_opts[index++] = app_params[i].short_option;
				if (app_params[i].mandatory_options) short_opts[index++] = ':';
			}
		}
		short_opts[index] = '\0';
	}

	return 0;
}

static int param_decode(char *opt_name, char *arg, struct param *param, int opt_index)
{
	switch (param->options[opt_index].type) {
		case option_int: {
				unsigned long val;
				if (xstrtoul(arg, &val)) {
					fprintf(stderr, "error: invalid integer value specified for --%s\n", opt_name);
			 	return -1;
 	 	}
 	 	if (param->n_cl_opts == 1 && param->context == 0 && (param->type == param_func || param->type == param_func_always)) {
 	 		param->context = (void *)val;
 	 	} else {
 	 		*(unsigned long *)param->options[opt_index].dest = val;
 	 	}
 	 	param->nparams++;
 	 }
 	 return 1;
		case option_string:
 	 	if (param->n_cl_opts == 1 && param->context == 0 && (param->type == param_func || param->type == param_func_always)) {
	 		param->context = (void *)arg;
	 	} else {
	 		*(char **)param->options[opt_index].dest = arg;
	 	}
	 	param->nparams++;
		 return 1;
	}
	return 0;
}

int parse_params(int argc, char **argv, struct param *params, int params_count)
{
	int i, j, error;
	int args_left;

	app_params = params;
	app_params_count = params_count;

	i = prepare_opts();
	if (i) return i;

	for (;;) {
		int res, longindex, parm_id;
		char *opt_name;
		char short_opt_in_str[3] = "  ";

		if (long_opt_count && short_opt_count) {
 		res = getopt_long(argc, argv, short_opts, (const struct option *)long_opts, &longindex);
		} else if (long_opt_count) {
			res = getopt_long_only(argc, argv, short_opts, (const struct option *)long_opts, &longindex);
		} else {
			res = getopt(argc, argv, short_opts);
		}

		if (res == -1) break;
		switch (res) {
		 case '?':
		  return 1;
		 case 1 ... ('?' - 1):
		 case ('?' + 1) ... 255:
				parm_id = short_opts_map[res];
				opt_name = short_opt_in_str;
				short_opt_in_str[1] = res;
				if (!app_params[parm_id].chosen) {
					app_params[parm_id].long_sel = 0;
				}
				break;
			default:
				parm_id = long_opts_map[res - START_LONG_OPT_ID];
				opt_name = app_params[parm_id].long_option;
				if (!app_params[parm_id].chosen) {
					app_params[parm_id].long_sel = 1;
				}
				break;
		}
		if (!app_params[parm_id].chosen) {
			app_params[parm_id].chosen = 1;
 		if (app_params[parm_id].mandatory_options) {
				error = param_decode(opt_name, optarg, app_params + parm_id, 0);
				if (error < 0) return 1;
			}
			int skip_opt_check = 0;
			for (i = 0; (i < MAX_OPTIONS) && (skip_opt_check==0); i++) {
				switch (app_params[parm_id].options[i].type) {
				 case option_string:
				 case option_int:
				  break;
				 case option_set_flag:
				 	*(int *)(app_params[parm_id].options[i].dest) = 1;
				  break;
				 case option_reset_flag:
				 	*(int *)(app_params[parm_id].options[i].dest) = 0;
				  break;
				 case option_set_var:
				 	*(void **)(app_params[parm_id].options[i].dest) = app_params[parm_id].options[i].val;
				  break;
				  default:
				  skip_opt_check = 1;
				  break;
				 //default: goto skip_opt_check;
				}
			}
//skip_opt_check:
		}
	}

	args_left = argc - optind;
	for (i = 0; i < app_params_count; i++) {

		if (app_params[i].chosen) {
			int from = app_params[i].nparams;
 		char *opt_name;
	 	char short_opt_in_str[3] = "  ";

			if (app_params[i].long_sel) {
				opt_name = app_params[i].long_option;
			} else {
				opt_name = short_opt_in_str;
				short_opt_in_str[1] = app_params[i].short_option;
			}

			if (args_left < (app_params[i].mandatory_options - app_params[i].nparams)) {

				args_left = 0;
				fprintf(stderr, "error: not enough parameters specified for --%s\n", opt_name);
				return 1;
			}
			for (j = from; j < MAX_OPTIONS && args_left > 0; j++) {
				error = param_decode(opt_name, argv[optind], app_params + i, j);
				if (error > 0) {
					args_left--;
					optind++;
				} else if (error < 0) {
				 return 1;
				}
 		}
		}
	}

	for (i = 0; i < app_params_count; i++) {
		if ((app_params[i].type & PARAM_TYPE_MASK) == param_func_always) {
			j = ((param_func_t)app_params[i].data)(app_params[i].context);
			if (j < 0) {
 			return j;
 		}
		}
	}

	for (i = 0; i < app_params_count; i++) {
		if (app_params[i].chosen && (app_params[i].type & PARAM_TYPE_MASK) == param_func) {
			return ((param_func_t)app_params[i].data)(app_params[i].context);
		}
	}

	if (default_param && (default_param->type & PARAM_TYPE_MASK) == param_func) {
	 return ((param_func_t)default_param->data)(default_param->context);
	}
	return 0;
}
