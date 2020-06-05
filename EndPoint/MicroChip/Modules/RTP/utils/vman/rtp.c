/*
 * $Id: rtp.c,v 1.1 2006/08/31 15:11:14 root Exp $
 */

#include "vman.h"
#include "rtperr.h"
#include <sys/time.h>

int err;

struct codec_type {
	char *name;
	int codec_id;
};

struct codec_type codec_types[] =
{
	{"g711", PAYLOAD_TYPE_G711A},
	{"g711u", PAYLOAD_TYPE_G711U},
	{"g729", PAYLOAD_TYPE_G729},
	{"g723", PAYLOAD_TYPE_G723},
	{"g711m", PAYLOAD_TYPE_G711A_MCC},
	{"g711um", PAYLOAD_TYPE_G711U_MCC},
	{NULL, -1},
};


static int parse_dest_string(char *dst, unsigned long *daddr, unsigned short *dport, unsigned short *sport);
static char const *ip_address(unsigned long addr);

static int get_pt(char *s)
{
	struct codec_type *entry;

	for (entry = codec_types; entry->name; entry++) {
		if (!strcmp(s, entry->name)) return entry->codec_id;
	}

	return -1;
}

int rtp_session(struct rtp_session *ses)
{
	struct rtp_connection_params parm;
	int chnid;
	struct rtp_channel_info chn_info;
	struct mix_slot slot;
	struct mix_slot_pair slot_pair;
	int tdm;


	if (parse_dest_string(ses->dest, &parm.send_addr, &parm.send_port, &parm.recv_port) < 0) {
		fprintf(stderr, "error: invalid \"dest\" specified\n");
		fflush(stderr);
		return -1;
	}

	int pt = get_pt(ses->payload);
	if (pt < 0) {
		fprintf(stderr, "error: invalid payload type specified (%s)\n", ses->payload);
		fflush(stderr);
		return -1;
	}

	parm.rx_pt = parm.tx_pt = pt;
	fprintf(stderr, "vman: payload type = %u\n", pt);

	open_rtp();

	parm.packet_factor=ses->pf;
	struct timeval tv;
	gettimeofday(&tv, 0);
	parm.ssrc = tv.tv_usec + tv.tv_sec * 1000;
	
	if (ses->attr) {	
		parm.vad = (strchr(ses->attr,'v')) ? 1 : 0;
		parm.rate = (strchr(ses->attr,'h')) ? 1 : 0;
	}

	if (ioctl(h_rtp, RTP_BEGIN_SESSION, &parm) < 0) {
	 	fprintf(stderr, "error: failed to begin rtp session (rtp error code = %d)\n", errno /*RTPStrError(parm.rtp_error)*/);
  		fflush(stderr);
	}
	

	chn_info.tdm = -1;
	chn_info.dev_no = -1;
	chn_info.chn = -1;
	chnid = do_ioctl(h_rtp, RTP_ADD_CHANNEL, &chn_info, "add channel");
	if(chnid < 0)
	{
		perror("do_ioctl RTP_ADD_CHANNEL");
		fflush(stderr);
		return -1;
	}

	open_ctl();
	tdm=chn_info.tdm;
	printf("add_channel return tdm=%d\n", tdm);
	if (tdm < 0)
	{
		open_conf();

		slot.dev_no = chn_info.dev_no;
		slot.type = slot_tdm;
		slot.number = chn_info.chn;

		do_ioctl(h_conf, MIX_RX_CLEAR, &slot, "MIX_RX_CLEAR");
		slot.type = slot_packet;
		do_ioctl(h_conf, MIX_RX_CLEAR, &slot, "MIX_RX_CLEAR");

		slot_pair.gain = 32000;

		slot_pair.mixer.dev_no = chn_info.dev_no;
		slot_pair.mixer.type = slot_tdm;
		slot_pair.mixer.number = chn_info.chn;
		
		slot_pair.input.dev_no = chn_info.dev_no;
		slot_pair.input.type = slot_packet;
		slot_pair.input.number = chn_info.chn;

		tdm = do_ioctl(h_conf, MIX_RX_ADD, &slot_pair, "MIX_RX_ADD");

		slot_pair.mixer.type = slot_packet;
		slot_pair.input.type = slot_tdm;

		do_ioctl(h_conf, MIX_RX_ADD, &slot_pair, "MIX_RX_ADD");

		close_conf();
	}

	open_hal();
	if(ses->channel >= 0x2000)
	{
		ses->channel&=~1;
		do_mux_connect(ses->channel, tdm);
		do_mux_connect(tdm, ses->channel+1);
	}
	else
	{
		//printf("vman: mux_connect: %u <=> %u\n", ses->channel, tdm);
		do_mux_connect(ses->channel, tdm);
		do_mux_connect(tdm, ses->channel);
	}
	close_hal();
	close_ctl();
	
	
	if (ses->attr) {
		open_dtmf(O_WRONLY);
		if (strchr(ses->attr,'e'))
			do_ioctl(h_dtmf, CHANNEL_LMS_ON, (void*)tdm, "echo on");
		else
			do_ioctl(h_dtmf, CHANNEL_LMS_OFF, (void*)tdm, "echo off");
		close_dtmf();
	}

	err=do_ioctl(h_rtp, RTP_START_TX, NULL, "rtp start tx");
	fprintf(stderr, "vman: pid = %u, chnid = %d\n", getpid(), chnid);
	fflush(stderr);

	for (;;) sleep(1);
	close_rtp();
	return 0;
}


static int parse_dest_string(char *dst, unsigned long *daddr, unsigned short *dport, unsigned short *sport)
{
 int slash_count, n;
 char *tmp, dhost[256];

 for (slash_count = 0, tmp = dst; tmp;)
 {
	tmp = strchr(tmp, '/'); 
	if (tmp)
	{
		tmp = tmp + 1;
		slash_count++;
	}
 }

 if (slash_count < 1 || slash_count > 2) return -1;

 n = strchr(dst, '/') - dst;
	if (!n) return -1;

 n = n > (sizeof(dhost) - 1) ? sizeof(dhost) - 1 : n;
	strncpy(dhost, dst, n);
	dhost[n] = 0;

	if ((*daddr = inet_addr(dhost)) == -1) {
  struct hostent *hep = gethostbyname(dhost);
		if (hep) {
			*daddr = ((struct in_addr *)(hep->h_addr_list[0]))->s_addr;
		}	else {
			fprintf(stderr, "error: failed to resolve name \"%s\"\n", dhost);
	  fflush(stderr);
			return -1;
		}
	}

	tmp = strchr(dst, '/') + 1;
	if (!*tmp || *tmp == '/') return -1;

	if (slash_count > 1) {
		*dport = strtoul(tmp, NULL, 0);
 	if (!*dport) {
	  fprintf(stderr, "error: invalid port number specified\n");
	  fflush(stderr);
		 return -1;
 	}

		tmp = strchr(tmp, '/') + 1;
		if (!*tmp) {
 		*sport = *dport;
		} else {
			*sport = strtoul(tmp, NULL, 0);
  	if (!*sport) {
	   fprintf(stderr, "error: invalid port number specified\n");
 	  fflush(stderr);
		  return -1;
 	 }
		}
	} else {
		*dport = strtoul(tmp, NULL, 0);
		if (!*dport) {
		 fprintf(stderr, "error: invalid port number specified\n");
	  fflush(stderr);
		 return -1;
		}
		*sport = *dport;
	}

	fprintf(stderr, "vman: dest parsed, ip=%s, dport=%u, sport=%u\n", ip_address(*daddr), *dport, *sport);
 fflush(stderr);

 return 0;
}

static char const *ip_address(unsigned long addr)
{
	static char ip_addr_buf[16];
	unsigned char *addr_u8 = (unsigned char *)&addr;

	sprintf(ip_addr_buf, "%u.%u.%u.%u", addr_u8[0], addr_u8[1], addr_u8[2], addr_u8[3]);
	return ip_addr_buf;
}

