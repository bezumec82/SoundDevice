/*
 * $Id: install.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include "vman.h"

int install_codec(struct install *i)
{
 struct vocoder_install_param ios;

 ios.codec_id = get_codec_id(i->codec_id);

	if (ios.codec_id <= 0) {
		fprintf(stderr, "error: invalid codec specified (%s)\n", i->codec_id);
 	return -1;
	}

	open_ctl();

 ios.dsp = i->dsp;

 do_ioctl(h_ctl, VOCODER_INSTALL, &ios, "install vocoder");

	close_ctl();

	printf("vman: codec %s installed on dsp %d\n", i->codec_id, i->dsp);

 return 0;
}

int uninstall_codec(int dsp)
{
 open_ctl();

 do_ioctl(h_ctl, VOCODER_UNINSTALL, &dsp, "uninstall vocoder");

	close_ctl();

	printf("vman: codec disabled on dsp %d\n", dsp);

 return 0;
}
