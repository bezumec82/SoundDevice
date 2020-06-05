#include "vman.h"
#include <string.h>

int mux_read_channel_state(unsigned channel)
{
	struct param_read_mux ios;
	int cmd;

	open_hal();
	cmd = HAL_MUX_READ_CM;
	ios.address = channel;

	do_ioctl(hal_ctl, cmd, &ios, "mux read");

	close_hal();

	return ios.data;
}


int __inline__ lmchannel(int f)
{
    /* if the source channel is local (bit 15=1), shift 11-7 bits right twice */
    return (f & 0x8000) ? ((f & 0x1f) | (f & 0x780) >> 2) : (0x2000 + (f & 0xfff));
}

void print_lm_reg(int data)
{
    if (data & 0x2000)
	printf("M:0x%2x\n", data & 0xff );
    else
	printf("OE:%1d, C:%d\n", (data>>12) & 1, lmchannel(data));
}

void print_bm_reg(unsigned int data)
{
    if (data & 0x1000) /*test 12 bit, if set the channel is in message mode */
	printf("M:0x%2x\n", data & 0xff);
    else
	printf("DC:%1d, C:%d\n", ((data>>11) & 1), ((data & 0x780) >> 2) + (data & 0x001f) );
}

int mux_list(char *filter)
{
    int message_mode=-1;
    int ignore_oe=0;
    int mm, oe, t, data;

    if(filter!=NULL)
    {
      if(strstr(filter, "m0")!=NULL) message_mode=0;
      if(strstr(filter, "m1")!=NULL) message_mode=1;
      if(strchr(filter, 'a')!=NULL) ignore_oe=1;
    }
    printf("Local connection memory:\n");
    for(t=0;t<0x1ff;++t)
    {
	data=mux_read_channel_state(t);
	mm=(data>>13) & 1;
	oe=(data>>12) & 1;

	if(oe || ignore_oe)
	 if(message_mode<0 || message_mode==mm)
		{
		    printf("%d\t",t); print_lm_reg(data);
		}
    }

    printf("\nBackplane connection memory:\n");
    for(t=0x4000; t<0x4000+0x800; ++t)
    {
	data=mux_read_channel_state(t);
	mm=(data>>12) &1;
	oe=(data>>11) &1;
	if(oe || ignore_oe)
	 if(message_mode<0 || message_mode==mm)
	 {
		 printf("%d\t",t-0x2000); print_bm_reg(data);
	 }
    }

    printf("\n");
	return 0;
}


int mux_read_dm(unsigned address)
{
    struct mux_msg_read param = { address, 0xFF };
    open_hal();
    param.channel=address;
    do_ioctl(hal_ctl, HAL_MUX_MSG_RD, &param, "HAL_MUX_MSG_RD");
    close_hal();
    return param.data;
}


int dump_mux_data_memory(struct dump_mux_dm *m)
{
	int count, addr, i;

	count = m->count <= 0 ? 16 : m->count;
	addr  = m->address;

	if ((m->address >= 0      && m->address+count <= 0x200) ||
	    (m->address >= 0x2000 && m->address+count <= 0x2800))
	    {
		printf("\n");
		for (i = 0; i < count; ++i, ++addr)
		{
		    if(i % 16==0) printf("0x%04x: ", addr);
		    if(i % 16==8) printf("  ");
		    printf("%02x ", mux_read_dm(addr));
		    if((i % 16)==15 || i==count-1 ) printf("\n");
		}

	    }
	else
	    {
		fprintf(stderr, "vman: dump: invalid address and/or size specified\n");
		return -1;
	    }

	return 0;
}
