/*
 * $Id: conf-mux.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include <linux/ioctl.h>
#include "vman.h"


int mux_rx_add(struct mux_rx_add *add)
{
	struct mix_slot_pair ios;
	memset(&ios, 0, sizeof(ios));
	ios.mixer.dev_no = ios.input.dev_no = add->dev_no;
	ios.mixer.type = ios.input.type = slot_tdm;

	ios.mixer.number = add->conf_no;
	ios.input.number = add->chn;
	ios.gain = add->gain;

	open_conf();
	do_ioctl(h_conf, MIX_RX_ADD, &ios, "MIX_RX_ADD");
	close_conf();
	return 0;
}

int mux_rx_remove(struct mux_rx_remove *rm)
{
	struct mix_slot_pair ios;
	memset(&ios, 0, sizeof(ios));

	ios.mixer.dev_no = ios.input.dev_no = rm->dev_no;
	ios.mixer.type = ios.input.type = slot_tdm;

	ios.mixer.number = rm->conf_no;
	ios.input.number = rm->chn;

	open_conf();
	do_ioctl(h_conf, MIX_RX_REMOVE, &ios, "MUX_RX_REMOVE");
	close_conf();
	return 0;
}

int mux_rx_clear(struct mux_rx_clear *clr)
{
    struct mix_slot ios;

    memset(&ios, 0, sizeof(ios));
    ios.dev_no = clr->dev_no;
//XXX: the number of conference mixers should not be constant! (16)
    if(clr->conf_no >= 16) {
		ios.type=slot_packet;
		ios.number=clr->conf_no-16;
    } else {	
        ios.type = slot_tdm;
		ios.number = clr->conf_no;
    }

    open_conf();
    do_ioctl(h_conf, MIX_RX_CLEAR, &ios, "MUX_RX_CLEAR");
    close_conf();
    return 0;
}

int mux_enum_devs(void)
{
	struct mix_enum ios;
	struct mix_query mq;
	int t, j;
	int printed;
	struct in_addr in;
	double gain;

	ios.number = -1;

	open_conf();

	while (ioctl(h_conf, MIX_ENUM_DEVS, &ios) >= 0) {
		char dsp_num_str[6];

		snprintf(dsp_num_str, sizeof(dsp_num_str), "dsp%d", ios.number);
		printf("%5s: confs = %u, TDMs = %u, nodes per conf = %u\n", dsp_num_str, ios.tdm_channels + ios.packet_channels, ios.tdm_channels, ios.channels_per_conf);
		fflush(stdout);

		for(t=0;t <ios.tdm_channels+ios.packet_channels;++t)
		{
			printed=0;
			for(j=0;j<ios.channels_per_conf;++j)
			{
				mq.dev_no=ios.number;
				mq.mixer=t;
				mq.entry=j;
				ioctl(h_conf, MIX_QUERY, &mq);

				if(mq.type==3) continue;

				if(!printed) {
							printf("\nconference %d:\n", t);
							fflush(stdout);
							printed=1;
				}
				gain=mq.gain&0x8000? 1.0 : 0.0;
				gain+=(mq.gain&0x7fff)/(float)0x8000;

				if(mq.type==0)
				{
					in.s_addr=mq.addr;
					printf("    [%d] RTP: %s/%d, gain=%f\n", j, inet_ntoa(in), mq.port, gain);
					fflush(stdout);
				}
				if(mq.type==1)
				{
					printf("    [%d] TDM: %d, gain=%f\n", j, mq.addr,  gain);
					fflush(stdout);
				}
			}
		}

	}
	printf("\n");
	close_conf();

	if (errno != ENODEV) {
		perror("MUX_ENUM_DEVS");
		fflush(stderr);
		return 1;
	}

	return 0;
}
