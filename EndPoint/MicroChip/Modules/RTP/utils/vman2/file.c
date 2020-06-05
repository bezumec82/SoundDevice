#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <errno.h>

#include "voice-ioctl.h"
#include "hal-ioctl.h"
 
const char *voice_dev_name = "/dev/voice";
const char *rtp_dev_name = "/dev/rtp";
const char *rtp_ext_dev_name = "/dev/rtp_ext";
const char *hal_dev_name = "/dev/hal";
const char *mixer_dev_name = "/dev/mixer";
const char *dtmf_dev_name = "/dev/dtmf";
const char *dsp_dev_name = "/dev/dsp";
const char *spi_tdm_dev_name = "/dev/spi_tdm";

/*----------------------------------------------------------------------------*/
static int open_dev(const char *name, int mode)
{
	int h = -1;
	h = open(name, mode);
	if (h < 0) 
	{
		fprintf(stderr, "failed to open %s: %s\n", name, strerror(errno));
	}
	return h;
}
/*----------------------------------------------------------------------------*/
int open_ctl(void)
{
	return open_dev(voice_dev_name, O_RDWR);
}
/*----------------------------------------------------------------------------*/
int open_spi_tdm(void)
{
	return open_dev(spi_tdm_dev_name, O_RDWR);
}
/*----------------------------------------------------------------------------*/
int open_hal(void)
{
	return open_dev(hal_dev_name, O_RDWR);
}
/*----------------------------------------------------------------------------*/
int open_conf(void)
{
	return open_dev(mixer_dev_name, O_RDWR);
}
/*----------------------------------------------------------------------------*/
int open_dsp(void)
{
	return open_dev(dsp_dev_name, O_RDWR);	
}
/*----------------------------------------------------------------------------*/
int open_dtmf(unsigned int mode)
{
	return open_dev(dtmf_dev_name, mode);
}
/*----------------------------------------------------------------------------*/
int open_rtp(void)
{
	return open_dev(rtp_dev_name, O_RDWR);
}
/*----------------------------------------------------------------------------*/
int open_rtp_ext(void)
{
	return open_dev(rtp_ext_dev_name, O_RDWR);
}
/*----------------------------------------------------------------------------*/
int do_ioctl(int h, unsigned ctl, void *arg, char *perror_msg)
{
    int err;

    if ((err = ioctl(h, ctl, arg)) < 0) {
    	/*printf("vman: do_ioctl FAIL: (0x%02X, 0x%02X, 0x%02X, %s) = %d\n", h, ctl, arg, perror_msg, err);*/
		if (perror_msg) {
			perror(perror_msg);
		}
    }

    /* printf("vman: do_ioctl OK: (0x%02X, 0x%02X, 0x%02X, %s) = %d\n", h, ctl, arg, perror_msg, err); */
    return err;
}
/*----------------------------------------------------------------------------*/
int do_mux_connect(int hal_ctl, unsigned dest, unsigned src)
{
	struct param_connect ios;

	ios.destination = dest;
	ios.source = src;

	do_ioctl(hal_ctl, HAL_CONNECT, &ios, "mux connect");

	return 0;
}
/*----------------------------------------------------------------------------*/
int do_mux_disconnect(int hal_ctl, unsigned chn)
{
	do_ioctl(hal_ctl, HAL_DISCONNECT, &chn, "mux disconnect");
	return 0;
}
/*----------------------------------------------------------------------------*/

