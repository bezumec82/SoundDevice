#include "vman.h"
#include <string.h>



int __inline__ lm_channel(int f)
{
	return (f & 0x8000) ? ((f & 0x1f) | (f & 0x780) >> 2) : (0x2000 + (f & 0xfff));
}

/*
int do_channelinfo()
{
	int tmp, tdm_to;
	struct voice_channel_info info;
	unsigned char *ip;
	char *codec;

	open_ctl();

	printf("active channels:\n");
	while(read(h_ctl, &info, sizeof(struct voice_channel_info)))
	{
		ip=(unsigned char *)&info.dst_addr;
		tmp=mux_read_channel_state(info.timeslot);
		if(((tmp>>13) & 1)) tdm_to=-1;
		else tdm_to=lm_channel(tmp);

		switch(info.codec_id)
		{
			case CODEC_ID_G711_MCC:
				codec="g711m";
				break;
			case CODEC_ID_G711:
			case CODEC_ID_G711_RTP:
				codec="g711";
				break;
			case CODEC_ID_G729:
			case CODEC_ID_G729_RTP:
				codec="g729";
				break;
			case CODEC_ID_G723:
			case CODEC_ID_G723_RTP:
				codec="g723";
				break;
			default:
				codec="unknown";
				break;
		}


		printf("%02d %02d %s\ttdm:%d<=%d sent:%d recv:%d dropd:%d\t%d.%d.%d.%d:%d/%d\n", info.dsp, info.slot, codec, \
			ip[0], ip[1], ip[2], ip[3], info.recv_port, info.dst_port, info.timeslot, \
			tdm_to, info.sent_packets, info.recv_packets, info.dropped_packets);
	}
	close_ctl();
	return 0;
}
*/
