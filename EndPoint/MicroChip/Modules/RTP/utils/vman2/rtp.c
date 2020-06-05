#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h> 
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "vman2.h"
#include "file.h"



struct s_codec_type {
	char *name;
	int codec_id;
};

struct s_codec_type codec_types[] =
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
/*----------------------------------------------------------------------------*/
static int get_pt(char *s)
{
	struct s_codec_type *entry;

	for (entry = codec_types; entry->name; entry++) {
		if (!strcmp(s, entry->name)) return entry->codec_id;
	}

	return -1;
}
/*----------------------------------------------------------------------------*/
int rtp_session_filter_set(struct rtp_session *prtp, struct rtp_filter_params	*parm)
{
	if (prtp)
	{
		if (parm)
		{
			memcpy(&prtp->filter_param, parm, sizeof(prtp->filter_param));
			if (ioctl(prtp->h_rtp, RTP_SET_FILTER, &prtp->filter_param) < 0) {
				fprintf(stderr, "error: failed to set filter session (rtp error code = %d)\n", errno);
				fflush(stderr);
			}
		}
		else
		{
			memset(&prtp->filter_param, 0, sizeof(prtp->filter_param));
			if (ioctl(prtp->h_rtp, RTP_RESET_FILTER, 0) < 0) {
				fprintf(stderr, "error: failed to reset filter session (rtp error code = %d)\n", errno);
				fflush(stderr);
			}
		}
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
int rtp_session_open(struct rtp_session *prtp, struct rtp_session_params *pparam)
{
	struct timeval tv;
	int err;
	int pt;

	if (prtp && pparam)
	{
		memcpy(&prtp->params, pparam, sizeof(*pparam));
		prtp->channel = pparam->channel;
		prtp->h_rtp = -1;
		prtp->h_ctl = -1;
		prtp->h_conf = -1;
		prtp->h_dtmf = -1;
		prtp->hal_ctl = -1;
		prtp->filter_param.recv_addr = 0;
		prtp->filter_param.ssrc = 0;
		
		if (parse_dest_string(pparam->dest, &prtp->conn_param.send_addr, &prtp->conn_param.send_port, &prtp->conn_param.recv_port) < 0) 
		{
			fprintf(stderr, "error: invalid \"dest\" specified\n");
			fflush(stderr);
			return -1;
		}
		
		pt = get_pt(pparam->payload);
		if (pt < 0)
		{
			fprintf(stderr, "error: invalid payload type specified (%s)\n", pparam->payload);
			fflush(stderr);
			return -1;
		}

		prtp->conn_param.rx_pt = prtp->conn_param.tx_pt = pt;
		fprintf(stderr, "vman: payload type = %u\n", pt);

		prtp->h_rtp = open_rtp();
		if(prtp->h_rtp < 0)
		{
			fprintf(stderr, "error: open_rtp");
			fflush(stderr);
			return -1;
		}

		prtp->conn_param.packet_factor=pparam->pf;		
		gettimeofday(&tv, 0);
		prtp->conn_param.ssrc = tv.tv_usec + tv.tv_sec * 1000;
		
		prtp->conn_param.vad = (strchr(pparam->attr,'v')) ? 1 : 0;
		prtp->conn_param.rate = (strchr(pparam->attr,'h')) ? 1 : 0;

		if (ioctl(prtp->h_rtp, RTP_BEGIN_SESSION, &prtp->conn_param) < 0) {
			fprintf(stderr, "error: failed to begin rtp session (rtp error code = %d)\n", errno);
			fflush(stderr);
			close(prtp->h_rtp);prtp->h_rtp=-1;
			return -1;
		}

		prtp->chn_info.tdm = -1;
		prtp->chn_info.dev_no = -1;
		prtp->chn_info.chn = -1;
		prtp->chnid = do_ioctl(prtp->h_rtp, RTP_ADD_CHANNEL, &prtp->chn_info, "add channel");
		if(prtp->chnid < 0)
		{
			fprintf(stderr, "error: do_ioctl RTP_ADD_CHANNEL");
			fflush(stderr);
			close(prtp->h_rtp);prtp->h_rtp=-1;
			return -1;
		}

		prtp->h_ctl = open_ctl();
		prtp->tdm=prtp->chn_info.tdm;
		fprintf(stdout, "add_channel return tdm=%d\n", prtp->tdm);
		if (prtp->tdm < 0)
		{
			prtp->h_conf = open_conf();

			prtp->slot.dev_no = prtp->chn_info.dev_no;
			prtp->slot.type = slot_tdm;
			prtp->slot.number = prtp->chn_info.chn;

			do_ioctl(prtp->h_conf, MIX_RX_CLEAR, &prtp->slot, "MIX_RX_CLEAR");
			prtp->slot.type = slot_packet;
			do_ioctl(prtp->h_conf, MIX_RX_CLEAR, &prtp->slot, "MIX_RX_CLEAR");

			prtp->slot_pair.gain = 32000;

			prtp->slot_pair.mixer.dev_no	= prtp->chn_info.dev_no;
			prtp->slot_pair.mixer.type		= slot_tdm;
			prtp->slot_pair.mixer.number	= prtp->chn_info.chn;
			
			prtp->slot_pair.input.dev_no	= prtp->chn_info.dev_no;
			prtp->slot_pair.input.type		= slot_packet;
			prtp->slot_pair.input.number	= prtp->chn_info.chn;

			prtp->tdm = do_ioctl(prtp->h_conf, MIX_RX_ADD, &prtp->slot_pair, "MIX_RX_ADD");

			prtp->slot_pair.mixer.type		= slot_packet;
			prtp->slot_pair.input.type		= slot_tdm;

			do_ioctl(prtp->h_conf, MIX_RX_ADD, &prtp->slot_pair, "MIX_RX_ADD");

			close(prtp->h_conf);prtp->h_conf=-1;
		}

		prtp->hal_ctl = open_hal();
		if(prtp->channel >= 0x2000)
		{
			prtp->channel&=~1;
			do_mux_connect(prtp->hal_ctl, prtp->channel, prtp->tdm);
			do_mux_connect(prtp->hal_ctl, prtp->tdm, prtp->channel+1);
		}
		else
		{
			if (prtp->channel >= 0)
			{
				//printf("vman: mux_connect: %u <=> %u\n", pparam->channel, tdm);
				do_mux_connect(prtp->hal_ctl, prtp->channel, prtp->tdm);
				do_mux_connect(prtp->hal_ctl, prtp->tdm, prtp->channel);
			}
		}
		
		close(prtp->hal_ctl);prtp->hal_ctl=-1;
		close(prtp->h_ctl);prtp->h_ctl=-1;
		
		
		prtp->h_dtmf = open_dtmf(O_WRONLY);
		if(prtp->h_dtmf < 0)
		{
			fprintf(stderr, "error: open dtmf");
			fflush(stderr);
		}		
		else
		{
			if (strchr(pparam->attr,'e'))
				do_ioctl(prtp->h_dtmf, CHANNEL_LMS_ON, (void*)prtp->tdm, "echo on");
			else
				do_ioctl(prtp->h_dtmf, CHANNEL_LMS_OFF, (void*)prtp->tdm, "echo off");
			close(prtp->h_dtmf);prtp->h_dtmf=-1;
		}


		err=do_ioctl(prtp->h_rtp, RTP_START_TX, NULL, "rtp start tx");
		fprintf(stdout, "rtp session id=%d\n", prtp->h_rtp);
	}

	return 0;
}
/*----------------------------------------------------------------------------*/
int rtp_session_close(struct rtp_session *prtp)
{
	if (prtp)
	{
		close(prtp->h_rtp);
		prtp->h_rtp = -1;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
int rtp_session_fshow(FILE * stream, struct rtp_session *prtp)
{
	if (stream && prtp)
	{
		fprintf(stream,"                              id : %d\n", prtp->h_rtp);
		fprintf(stream,"[config]                 payload : '%s'\n", prtp->params.payload);
		fprintf(stream,"[config]                    dest : '%s'\n", prtp->params.dest);
		fprintf(stream,"[config]                 channel : %d\n", prtp->params.channel);
		fprintf(stream,"[config]                    attr : '%s'\n", prtp->params.attr);
		fprintf(stream,"[config]                      pf : %d\n", prtp->params.pf);
		fprintf(stream,"[config]               filter IP : '%s'\n", ip_address(prtp->filter_param.recv_addr));
		fprintf(stream,"[config]             filter SSRC : 0x%08lX\n", prtp->filter_param.ssrc);
		
		fprintf(stream,"[state]                  channel : %d\n", prtp->channel);
		fprintf(stream,"[state]                      tdm : %d\n", prtp->tdm);
		
		
		if (ioctl(prtp->h_rtp, RTP_STAT_INFO, &prtp->stat_info) < 0) {
			fprintf(stream, "[state]    read stat info error : %d)\n", errno);
			fflush(stream);
		}
		else
		{
			fprintf(stream,"[state]                 tx count : %ld\n", prtp->stat_info.ptx);
			fprintf(stream,"[state]                 rx count : %ld\n", prtp->stat_info.prx);
			fprintf(stream,"[state]         tx dropped count : %ld\n", prtp->stat_info.tx_dropped);
			fprintf(stream,"[state]         rx dropped count : %ld\n", prtp->stat_info.rx_dropped);
			
			fprintf(stream,"[state]         tx last time ago : %ld, ms\n", prtp->stat_info.tx_last);
			fprintf(stream,"[state]         rx last time ago : %ld, ms\n", prtp->stat_info.rx_last);
		}
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static int parse_dest_string(char *dst, unsigned long *daddr, unsigned short *dport, unsigned short *sport)
{
 int slash_count, n;
 char *tmp, dhost[256];

 for (slash_count = 0, tmp = dst; (tmp = strchr(tmp, '/')); tmp = tmp + 1, slash_count++){;}

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
/*----------------------------------------------------------------------------*/
static char const *ip_address(unsigned long addr)
{
	static char ip_addr_buf[16];
	unsigned char *addr_u8 = (unsigned char *)&addr;

	sprintf(ip_addr_buf, "%u.%u.%u.%u", addr_u8[0], addr_u8[1], addr_u8[2], addr_u8[3]);
	return ip_addr_buf;
}
/*----------------------------------------------------------------------------*/
static int get_next_param(char **str_params, char *str_value, int size_value)
{
	int res = 0;
	char *str_params_new;
	
	if (str_params && *str_params && **str_params)
	{
		memset(str_value, 0, size_value);
		memcpy(str_value, *str_params, size_value-1);
		str_params_new = strchr(*str_params, '/');
		if (str_params_new)
		{
			str_value[str_params_new-*str_params] = '\0';
			*str_params = str_params_new;
		}
		*str_params=*str_params+1;
		res = 1;
	}
	return res;
}
/*----------------------------------------------------------------------------*/
int parse_filter_param(char *str_filter, struct rtp_filter_params *parm)
{
	int res = 0;
	char *slash, str_value[256];
	int value;
	struct in_addr	session_addr;
	
	slash = str_filter;
	
	/* session_addr */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		res++;
		
		if (parm)
		{
			inet_pton(AF_INET, str_value, &session_addr);
			parm->recv_addr = session_addr.s_addr;
		}
	}
	
	/* ssrc */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;
		
		if (parm)
		{
			parm->ssrc = value;
		}
	}

	return res;
}
/*----------------------------------------------------------------------------*/

