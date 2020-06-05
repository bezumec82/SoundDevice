/*
 * $Id: mux.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include "vman.h"

inline void mux_clear_region(unsigned from, int number)
{
	int i;
	open_hal();
	for(i=from; i< (from+number); ++i)
		do_mux_disconnect(i);
	close_hal();
}

int mux_clear(char *region)
{
	const char mc_all[]="all";
	const char mc_backplane[]="backplane";
	const char mc_local[]="local";

	if(region)
	{
		if(!strcasecmp(region, mc_backplane))
		{
			printf("clearing backplane field...\n");
			mux_clear_region(8192, 2048);
			return 0;
		}
		if(!strcasecmp(region, mc_local))
		{
			printf("clearing local field...\n");
			mux_clear_region(0, 512);
			return 0;
		}
		if(!strcasecmp(region, mc_all))
		{
			printf("clearing both local and backplane fields...\n");
			mux_clear_region(8192, 2048);
			mux_clear_region(0, 512);
		}
	}
	return 0;
}



int do_mux_connect(unsigned dest, unsigned src)
{
	struct param_connect ios;

	ios.destination = dest;
	ios.source = src;

	do_ioctl(hal_ctl, HAL_CONNECT, &ios, "mux connect");

	return 0;
}

int do_mux_disconnect(unsigned chn)
{
	do_ioctl(hal_ctl, HAL_DISCONNECT, &chn, "mux disconnect");
	return 0;
}


int mux_connect(struct mux_conn *c)
{
	open_hal();
	do_mux_connect(c->dest, c->src);
	close_hal();
	return 0;
}

int mux_disconnect(unsigned int chn)
{
	open_hal();

	do_mux_disconnect(chn);
	close_hal();

	return 0;
}

extern void print_lm_reg(int data);
extern void print_bm_reg(int data);

int mux_read_mem(struct mux_read_mem *mem)
{
	struct param_read_mux ios;
	int cmd;
	char *mem_type_str;

	open_hal();

	if (mem->mem_type)
	{
		cmd = HAL_MUX_READ_DM;
		mem_type_str = "dm";
	}
	else
	{
		cmd = HAL_MUX_READ_CM;
		mem_type_str = "cm";
	}

	ios.address = mem->channel;

	do_ioctl(hal_ctl, cmd, &ios, "mux read");

	close_hal();

	printf("vman: mux_%s(0x%x) = 0x%x\n", mem_type_str, mem->channel, ios.data);
	if (!mem->mem_type)
	{
		if (mem->channel < 0x200)
			print_lm_reg(ios.data);
		else
			print_bm_reg(ios.data);
	}

	return 0;
}



int mux_read_msg(unsigned channel)
{
	struct mux_msg_read param = { channel, 0xFF };

	open_hal();

	do_ioctl(hal_ctl, HAL_MUX_MSG_RD, &param, "HAL_MUX_MSG_RD");

	close_hal();

	printf("vman: mux(%u) = 0x%02X\n", param.channel, (__u8) param.data);

	return 0;
}

int mux_write_msg(struct mux_msg_write *mw)
{
	open_hal();

	do_ioctl(hal_ctl, HAL_MUX_MSG_WR, mw, "HAL_MUX_MSG_WR");

	close_hal();

	printf("vman: mux(%u) = 0x%02X\n", mw->channel, mw->data);

	return 0;
}

void enable_panic()
{
	open_hal();
	do_ioctl(hal_ctl, HAL_MUX_ON_FATAL_ERROR, (void *)HAL_FATAL_ERROR_KERNEL_PANIC, "HAL_MUX_ON_FATAL_ERROR: HAL_FATAL_ERROR_KERNEL_PANIC");
	close_hal();
}

void disable_panic()
{
	open_hal();
	do_ioctl(hal_ctl, HAL_MUX_ON_FATAL_ERROR, (void *)HAL_FATAL_ERROR_IGNORE, "HAL_MUX_ON_FATAL_ERROR: HAL_FATAL_ERROR_IGNORE");
	close_hal();
}
