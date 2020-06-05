/*
 * $Id: conf.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include "vman.h"

int conf_list(void)
{
	struct conf_enum_dsp_param ios;
	int h;

	ios.dsp = -1;

	h = open("/dev/conf", 0);
	if (h < 0) {
		perror("open conference");
		return 1;
	}

	while (ioctl(h, CONF_ENUM_DSP, &ios) >= 0) {
		char dsp_num_str[6];

		snprintf(dsp_num_str, sizeof(dsp_num_str), "dsp%d", ios.dsp);

	 printf("%+5s: nodes = %u, confs = %u, nodes per conf = %u\n", dsp_num_str, ios.max_chns, ios.max_confs, ios.max_conf_chns);
	 fflush(stdout);
	}

	if (errno != ENODEV) {
		close(h);
		perror("CONF_ENUM_DSP");
		return 1;
	}

	close(h);

	return 0;
}

int conf_add(struct conf_add_param *ca)
{
	int h, in_tdm, out_tdm;
	struct conf_output_tdm cot;


	h = open("/dev/conf", 0);
	if (h < 0) {
		perror("open conference");
		return 1;
	}

	cot.dsp = ca->dsp;
	cot.conf = ca->conf;

	out_tdm = ioctl(h, CONF_OUTPUT_TDM, &cot);

	if ((in_tdm = ioctl(h, CONF_ADD, ca)) < 0) {
		close(h);
		perror("CONF_ADD");
		return 1;
	} else {
		printf("vman: node %u added to conference %u on dsp %u (in_tdm = %d, out_tdm = %d)\n", ca->chn, ca->conf, ca->dsp, in_tdm, out_tdm);
	}

	close(h);

	return 0;
}

int conf_remove(struct conf_remove_param *cr)
{
	int h;

	h = open("/dev/conf", 0);
	if (h < 0) {
		perror("open conference");
		return 1;
	}

	if (ioctl(h, CONF_REMOVE, cr) < 0) {
		close(h);
		perror("CONF_REMOVE");
		return 1;
	} else {
		printf("vman: node %u removed from conference on dsp %u\n", cr->chn, cr->dsp);
	}

	close(h);

	return 0;
}

int conf_clean(struct conf_clean_param *cc)
{
	int h;

	h = open("/dev/conf", 0);
	if (h < 0) {
		perror("open conference");
		return 1;
	}

	if (ioctl(h, CONF_CLEAN, cc) < 0) {
		close(h);
		perror("CONF_CLEAN");
		return 1;
	} else {
		printf("vman: conference %u cleaned up on dsp %u\n", cc->conf, cc->dsp);
	}

	close(h);

	return 0;
}
