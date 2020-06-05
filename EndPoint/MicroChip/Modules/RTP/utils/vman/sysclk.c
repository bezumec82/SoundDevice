/*
 * $Id: sysclk.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include "vman.h"


int show_sysclk_status(void)
{
	struct timespec delay, rem, rem1;
	struct param_get_sysclk_state ios;
	int prev_state, first_time;
	unsigned long prev_freq;

	open_hal();

	delay.tv_sec = 0;
	delay.tv_nsec = 500000000;

	first_time = 1;
	prev_state = 0;
	prev_freq = 0;

	while (1) {
	 if (ioctl(hal_ctl, HAL_GETSYSCLK_STATE, &ios) < 0) {
	 	perror("HW_SYSCLK_STATE");
	 	return 1;
	 }

		if (first_time || prev_state != ios.prognosed_state || prev_freq != ios.freq) {
			first_time = 0;
			prev_state = ios.prognosed_state;
			prev_freq = ios.freq;
			printf("vman: sysclock %s, freq %lu\n", prev_state ? "ok" : "fail", prev_freq);
		}

		if (nanosleep(&delay, &rem)) {
			nanosleep(&rem, &rem1);
		}
	}

	close_hal();
	return 0;
}
