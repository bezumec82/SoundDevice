#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>

#include "file.h"
#include "mux.h"

int mux_clear_region(unsigned int from, unsigned int number);
/* -------------------------------------------------------------------------- */
int mux_clear(void)
{
	fprintf(stderr, "clearing all mux connections...\n");
	fflush(stderr);
	mux_clear_region(0, 64);
	return 0;
}
/* -------------------------------------------------------------------------- */
int mux_connect(struct s_mux_conn *c)
{
	int hal_ctl;
	hal_ctl = open_hal();
	if (hal_ctl < 0)
	{
		fprintf(stderr, "error: open hal '%s(%d)'\n", strerror(errno), errno);
		fflush(stderr);
	}
	else
	{
		do_mux_connect(hal_ctl, c->dest, c->src);		
		close(hal_ctl);
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int mux_disconnect(unsigned int chn)
{
	int hal_ctl;
	hal_ctl = open_hal();
	if (hal_ctl < 0)
	{
		fprintf(stderr, "error: open hal '%s(%d)'\n", strerror(errno), errno);
		fflush(stderr);
	}
	else
	{
		do_mux_disconnect(hal_ctl, chn);
		close(hal_ctl);
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int mux_clear_region(unsigned int from, unsigned int number)
{
	int chn;
	int hal_ctl;
	hal_ctl = open_hal();
	if (hal_ctl < 0)
	{
		fprintf(stderr, "error: open hal '%s(%d)'\n", strerror(errno), errno);
		fflush(stderr);
	}
	else
	{
		for(chn=from; chn< (from+number); ++chn)
		{
			do_mux_disconnect(hal_ctl, chn);
		}
		close(hal_ctl);
	}
	return 0;	
}
/* -------------------------------------------------------------------------- */
