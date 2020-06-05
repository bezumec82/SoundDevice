/*
 * $Id: clock.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include "vman.h"

int voice_set_clock_source(char *clock_source)
{
	//u_int clk_src;

	if (clock_source == NULL || !strcmp(clock_source, "-") || !clock_source[0])
	{
		struct param_get_clock clk_src;
		open_hal();
		do_ioctl(hal_ctl, HAL_GETCLOCK, &clk_src, "get clock source");
		close_hal();

		fprintf(stderr, "vman: clock = %s\n", clk_srcs[clk_src.clock & CLK_SEL_MASK]);
		return 0;
	}
	else
	{
		int i;

		for (i = 0; i < clk_srcs_count; i++)
		{
			if (!strcmp(clk_srcs[i], clock_source)) goto clock_src_valid;
		}
		fprintf(stderr, "error: invalid clock source specified\n");
		return 1;

clock_src_valid:
		open_hal();

		struct param_get_clock clk_src;

		//if (i == CLK_SEL_FREERUN) i |= CLK_TCLK_EN;
		clk_src.clock = i;

		do_ioctl(hal_ctl, HAL_SETCLOCK, &clk_src, "select clock source");
		close_hal();

		fprintf(stderr, "vman: clock is set to %s\n", clk_srcs[clk_src.clock & CLK_SEL_MASK]);

		return 0;
	}
}
