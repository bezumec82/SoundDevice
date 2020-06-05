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
/*----------------------------------------------------------------------------*/
int rtp_ext_session_open(struct rtp_ext_session *prtpe, struct rtp_ext_session_params *pparam)
{
	int res;
	if (prtpe && pparam)
	{
		memcpy(&prtpe->params, pparam, sizeof(*pparam));
		prtpe->h_rtp_ext = -1;
		
		prtpe->conn_param.etype = SIG_PROTO_PROFILE_ID;
		prtpe->conn_param.fd_rtp = prtpe->params.h_rtp;
		
		sig_proto_init(&prtpe->proto_rx);
		sig_proto_init(&prtpe->proto_tx);
		
		prtpe->h_rtp_ext = open_rtp_ext();
		if(prtpe->h_rtp_ext < 0)
		{
			fprintf(stderr, "error: open rtp_ext");
			fflush(stderr);
			return -1;
		}
		
		res = do_ioctl(prtpe->h_rtp_ext, RTP_EXT_ATTACH_SESSION, &prtpe->conn_param, "rtp_ext attach");
		if(res < 0)
		{
			rtp_ext_session_close(prtpe);
			return -1;
		}
		
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
int rtp_ext_session_close(struct rtp_ext_session *prtpe)
{
	int res;
	
	res = do_ioctl(prtpe->h_rtp_ext, RTP_EXT_DETACH_SESSION, &prtpe->conn_param, "rtp_ext detach");
	close(prtpe->h_rtp_ext);
	prtpe->h_rtp_ext = -1;
	return 0;
}
/*----------------------------------------------------------------------------*/
static int show_rtpe_stat(FILE * stream, struct rtp_ext_session *prtpe, unsigned char id, char *id_string)
{
	unsigned int count_recv, count_send;
	
	if (prtpe && id_string)
	{
		count_recv	= prtpe->state.packets_recv_count[id & 0xFF];
		count_send	= prtpe->state.packets_send_count[id & 0xFF];
		
		fprintf(stream," %*s : %*d / %*d\n",
						24, id_string,
						5, count_recv, 5, count_send);
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static char sig_proto_buffer[4096]={0};
int rtp_ext_session_fshow(FILE * stream, struct rtp_ext_session *prtpe)
{
	char str_session_addr[32];
	if (stream && prtpe)
	{
		fprintf(stream,"                              id : %d\n", prtpe->h_rtp_ext);
		fprintf(stream,"         attached rtp session id : %d\n", prtpe->conn_param.fd_rtp);
		fprintf(stream,"                      poll count : %ld\n", prtpe->poll_count);
		fprintf(stream,"         total read ioctl packets: %ld\n", prtpe->total_read_ioctl_packets);
		fprintf(stream,"               total read packets: %ld\n", prtpe->total_read_packets);
		fprintf(stream,"              total write packets: %ld\n", prtpe->total_write_packets);
		fprintf(stream,"                 total read bytes: %ld\n", prtpe->total_read_bytes);
		fprintf(stream,"                total write bytes: %ld\n", prtpe->total_write_bytes);
		fprintf(stream,"         sig proto tx last error : %d\n", prtpe->sig_proto_tx_last_error);
		fprintf(stream,"         sig proto rx last error : %d\n", prtpe->sig_proto_rx_last_error);
		fprintf(stream,"                last error write : %d\n", prtpe->last_error_write);
		fprintf(stream,"                 last error read : %d\n", prtpe->last_error_read);
		fprintf(stream,"                last error ioctl : %d\n", prtpe->last_error_ioctl);
		fprintf(stream,"\n---------------------------------------\n");
		fprintf(stream,"\n                       Statistic :\n");
		fprintf(stream,"\n                          receice / send\n");
		
		show_rtpe_stat(stream, prtpe, WM_FLAG, 			"WM_FLAG");
		show_rtpe_stat(stream, prtpe, WM_TRANS, 		"WM_TRANS");
		show_rtpe_stat(stream, prtpe, WM_RADIO_QUALITY, "WM_RADIO_QUALITY");
		show_rtpe_stat(stream, prtpe, WM_CHAN, 			"WM_CHAN");
		show_rtpe_stat(stream, prtpe, WM_SELECT_RC_CHAN,"WM_SELECT_RC_CHAN");
		show_rtpe_stat(stream, prtpe, WM_SEL_CALL_TRANS,"WM_SEL_CALL_TRANS");
		fprintf(stream,"---------------------------------------\n");
		show_rtpe_stat(stream, prtpe, GW_FLAG, 			"GW_FLAG");
		show_rtpe_stat(stream, prtpe, GW_ADJUST_CHAN, 	"GW_ADJUST_CHAN");
		show_rtpe_stat(stream, prtpe, GW_FLAG_PTT, 		"GW_FLAG_PTT");
		show_rtpe_stat(stream, prtpe, GW_FLAG_BSS, 		"GW_FLAG_BSS");
		show_rtpe_stat(stream, prtpe, GW_FLAG_RV, 		"GW_FLAG_RV");
		show_rtpe_stat(stream, prtpe, GW_FLAG_SH, 		"GW_FLAG_SH");
		show_rtpe_stat(stream, prtpe, GW_FLAG_PW, 		"GW_FLAG_PW");
		show_rtpe_stat(stream, prtpe, GW_FLAG_RS, 		"GW_FLAG_RS");
		show_rtpe_stat(stream, prtpe, GW_SEL_CALL, 		"GW_SEL_CALL");
		show_rtpe_stat(stream, prtpe, GW_SELECT_RC_CHAN,"GW_SELECT_RC_CHAN");
		fprintf(stream,"\n---------------------------------------\n");
		
		fprintf(stream,"\n                        State GW :\n");
		fprintf(stream,"                        priority : %ld\n", prtpe->state.gw_state.priority);
		fprintf(stream,"                          flag_t : %ld\n", prtpe->state.gw_state.flag_t);
		fprintf(stream,"                          flag_p : %ld\n", prtpe->state.gw_state.flag_p);
		fprintf(stream,"                          flag_r : %ld\n", prtpe->state.gw_state.flag_r);
		fprintf(stream,"                          flag_s : %ld\n", prtpe->state.gw_state.flag_s);
		fprintf(stream,"                          flag_b : %ld\n", prtpe->state.gw_state.flag_b);
		fprintf(stream,"                         flag_rv : %ld\n", prtpe->state.gw_state.flag_rv);
		fprintf(stream,"                         flag_sh : %ld\n", prtpe->state.gw_state.flag_sh);
		fprintf(stream,"                         flag_rs : %ld\n", prtpe->state.gw_state.flag_rs);
		fprintf(stream,"                         flag_pw : %ld\n", prtpe->state.gw_state.flag_pw);
		fprintf(stream,"                          flag_c : %ld\n", prtpe->state.gw_state.flag_c);
		fprintf(stream,"                          status : %ld\n\n", prtpe->state.gw_state.status);
		fprintf(stream,"                      channel ID : %ld\n", prtpe->state.gw_state.chan_id);
		fprintf(stream,"                    channel Freq : %4.6f\n", prtpe->state.gw_state.chan_freq);
		fprintf(stream,"                    RC interface : %d\n", prtpe->state.gw_state.rc_interface);
		fprintf(stream,"                      RC channel : %d\n", prtpe->state.gw_state.rc_channel);
		fprintf(stream,"                        callerID : '%s'\n", prtpe->state.gw_state.callerID);
		
		fprintf(stream,"\n---------------------------------------\n");
		fprintf(stream,"\n                        State WM :\n");
		fprintf(stream,"                        priority : %ld\n", prtpe->state.wm_state.priority);
		fprintf(stream,"                          flag_t : %ld\n", prtpe->state.wm_state.flag_t);
		fprintf(stream,"                          flag_p : %ld\n", prtpe->state.wm_state.flag_p);
		fprintf(stream,"                          flag_r : %ld\n", prtpe->state.wm_state.flag_r);
		fprintf(stream,"                          flag_s : %ld\n", prtpe->state.wm_state.flag_s);
		fprintf(stream,"                          flag_b : %ld\n", prtpe->state.wm_state.flag_b);
		fprintf(stream,"                         flag_rv : %ld\n", prtpe->state.wm_state.flag_rv);
		fprintf(stream,"                         flag_sh : %ld\n", prtpe->state.wm_state.flag_sh);
		fprintf(stream,"                         flag_rs : %ld\n", prtpe->state.wm_state.flag_rs);
		fprintf(stream,"                         flag_pw : %ld\n", prtpe->state.wm_state.flag_pw);
		fprintf(stream,"                          flag_c : %ld\n", prtpe->state.wm_state.flag_c);
		fprintf(stream,"                          status : %ld\n\n", prtpe->state.wm_state.status);
		fprintf(stream,"                      channel ID : %ld\n", prtpe->state.wm_state.chan_id);
		fprintf(stream,"                    channel Freq : %4.6f\n", prtpe->state.wm_state.chan_freq);
		fprintf(stream,"                   Radio Quality : %4.6f\n", prtpe->state.wm_state.radio_quality);
		fprintf(stream,"                    RC interface : %d\n", prtpe->state.wm_state.rc_interface);
		fprintf(stream,"                      RC channel : %d\n", prtpe->state.wm_state.rc_channel);			
		memset(str_session_addr, 0, sizeof(str_session_addr));
		inet_ntop(AF_INET, &prtpe->state.wm_state.session_addr, str_session_addr, sizeof(str_session_addr)-1);	
		fprintf(stream,"                    Session Addr : '%s'\n", str_session_addr);
		fprintf(stream,"                            SSRC : 0x%08lX\n", prtpe->state.wm_state.ssrc);
		fprintf(stream,"                        callerID : '%s'\n", prtpe->state.wm_state.callerID);
		
		fprintf(stream,"\n---------------------------------------\n");
		fprintf(stream,"                     last rx SSRC: 0x%08lX\n", prtpe->state.rtp_ssrc);
		memset(str_session_addr, 0, sizeof(str_session_addr));		
		inet_ntop(AF_INET, &prtpe->state.rtp_session_addr, str_session_addr, sizeof(str_session_addr)-1);
		fprintf(stream,"            last rx Session Addr : '%s'\n", str_session_addr);
		
		sprintf_sig_proto(sig_proto_buffer, sizeof(sig_proto_buffer), &prtpe->proto_rx, SIG_PROTO_SPRINTF_MODE_PRETTY);
		fprintf(stream,"    last proto rx:\n%s\n", sig_proto_buffer);
		sprintf_sig_proto(sig_proto_buffer, sizeof(sig_proto_buffer), &prtpe->proto_tx, SIG_PROTO_SPRINTF_MODE_PRETTY);
		fprintf(stream,"    last proto tx:\n%s\n", sig_proto_buffer);
		
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
int rtp_ext_session_read(struct rtp_ext_session *prtpe)
{
	int res = 0, total_read = 0;
	int res_unpack = 0;
	unsigned char id;
	if (prtpe)
	{
		if (prtpe->h_rtp_ext > 0)
		{
			do
			{
				res = ioctl(prtpe->h_rtp_ext, RTP_EXT_READ_SOURCE_ADDR, &prtpe->rx_saddr);
				if (res > 0)
				{
					prtpe->total_read_ioctl_packets++;
					prtpe->state.rtp_ssrc = prtpe->rx_saddr.ssrc;
					prtpe->state.rtp_session_addr.s_addr = prtpe->rx_saddr.src_addr;
				}
				else
				{
					if (res < 0)
						prtpe->last_error_ioctl = errno;
				}
				
				res = read(prtpe->h_rtp_ext, prtpe->rx_payload, sizeof(prtpe->rx_payload));
				if (res > 0)
				{
					total_read += res;
					prtpe->total_read_bytes += res;
					sig_proto_init(&prtpe->proto_rx);
					res_unpack = sig_proto_unpack(&prtpe->proto_rx, (unsigned char *)prtpe->rx_payload, res);
					if (res_unpack == SIG_PROTO_UNPACK_OK)
					{
						prtpe->total_read_packets++;
						id = prtpe->proto_rx.type;
						prtpe->state.packets_recv_count[id]++;
						
						rtp_ext_session_process(prtpe, &prtpe->proto_rx);
					}
					else
					{
						prtpe->sig_proto_rx_last_error = res_unpack;
					}
				}
				else
				{
					if (res < 0)
						prtpe->last_error_read = errno;
				}
			}
			while (res > 0);
			if (res == 0)
				res = total_read;
		}
	}
	return res;
}
/*----------------------------------------------------------------------------*/
int rtp_ext_session_write(struct rtp_ext_session *prtpe, struct sig_proto *pproto)
{
	int res = 0;
	int res_pack = 0;
	unsigned char id;
	if (prtpe && pproto)
	{
		memcpy(&prtpe->proto_tx, pproto, sizeof(*pproto));
		res_pack = sig_proto_pack(&prtpe->proto_tx, (unsigned char *)prtpe->tx_payload, sizeof(prtpe->tx_payload));
		if (res_pack > 0)
		{
			res = write(prtpe->h_rtp_ext, prtpe->tx_payload, res_pack);
			if (res > 0)
			{
				prtpe->total_write_bytes += res;
				prtpe->total_write_packets++;
				id = prtpe->proto_tx.type;
				prtpe->state.packets_send_count[id]++;
			}
			else
			{
				if (res < 0)
					prtpe->last_error_write = errno;
			}
		}
		else
		{
			prtpe->sig_proto_tx_last_error = res_pack;
		}
	}
	return res;
}
/*----------------------------------------------------------------------------*/
int rtp_ext_session_process(struct rtp_ext_session *prtpe, struct sig_proto *pproto)
{
	if (prtpe)
	{
		/* process proto */
		if (pproto)
		{
			/* gw state */
			if (
				(pproto->type == GW_FLAG) ||
				(pproto->type == GW_ADJUST_CHAN) ||
				(pproto->type == GW_FLAG_PTT) ||
				(pproto->type == GW_FLAG_BSS) ||
				(pproto->type == GW_FLAG_RV) ||
				(pproto->type == GW_FLAG_SH) ||
				(pproto->type == GW_FLAG_PW) ||
				(pproto->type == GW_FLAG_RS) ||
				(pproto->type == GW_SEL_CALL) ||
				(pproto->type == GW_SELECT_RC_CHAN)
			)
			{
				prtpe->state.gw_state.priority = pproto->flags.priority;
				prtpe->state.gw_state.flag_t = pproto->flags.flag_t;
				prtpe->state.gw_state.flag_p = pproto->flags.flag_p;
				prtpe->state.gw_state.flag_s = pproto->flags.flag_r;
				prtpe->state.gw_state.flag_s = pproto->flags.flag_s;
				prtpe->state.gw_state.flag_b = pproto->flags.flag_b;
				prtpe->state.gw_state.flag_rv = pproto->flags.flag_rv;
				prtpe->state.gw_state.flag_sh = pproto->flags.flag_sh;
				prtpe->state.gw_state.flag_rs = pproto->flags.flag_rs;
				prtpe->state.gw_state.flag_pw = pproto->flags.flag_pw;
				prtpe->state.gw_state.flag_c = pproto->flags.flag_c;
				prtpe->state.gw_state.status = pproto->flags.status;
				
				if (pproto->type == GW_ADJUST_CHAN)
				{
					prtpe->state.gw_state.chan_id = pproto->payload.gw_adjust_chan.id;
					prtpe->state.gw_state.chan_freq = pproto->payload.gw_adjust_chan.freq;
				}
				
				if (pproto->type == GW_SEL_CALL)
				{
					memset(prtpe->state.gw_state.callerID, 0, sizeof(prtpe->state.gw_state.callerID));
					memcpy(prtpe->state.gw_state.callerID, pproto->payload.gw_sel_call.callerID, pproto->payload.gw_sel_call.callerID_length);
				}
				
				if (pproto->type == GW_SELECT_RC_CHAN)
				{
					prtpe->state.gw_state.rc_interface = pproto->payload.gw_select_rc_chan.interface;
					prtpe->state.gw_state.rc_channel = pproto->payload.gw_select_rc_chan.channel;
				}
			}

			/* wm state */
			if (
				(pproto->type == WM_FLAG) ||
				(pproto->type == WM_TRANS) ||
				(pproto->type == WM_RADIO_QUALITY) ||
				(pproto->type == WM_CHAN) ||
				(pproto->type == WM_SEL_CALL_TRANS) ||
				(pproto->type == WM_SELECT_RC_CHAN)
			)
			{
				prtpe->state.wm_state.priority = pproto->flags.priority;
				prtpe->state.wm_state.flag_t = pproto->flags.flag_t;
				prtpe->state.wm_state.flag_p = pproto->flags.flag_p;
				prtpe->state.wm_state.flag_s = pproto->flags.flag_r;
				prtpe->state.wm_state.flag_s = pproto->flags.flag_s;
				prtpe->state.wm_state.flag_b = pproto->flags.flag_b;
				prtpe->state.wm_state.flag_rv = pproto->flags.flag_rv;
				prtpe->state.wm_state.flag_sh = pproto->flags.flag_sh;
				prtpe->state.wm_state.flag_rs = pproto->flags.flag_rs;
				prtpe->state.wm_state.flag_pw = pproto->flags.flag_pw;
				prtpe->state.wm_state.flag_c = pproto->flags.flag_c;
				prtpe->state.wm_state.status = pproto->flags.status;
				
				if (pproto->type == WM_CHAN)
				{
					prtpe->state.wm_state.chan_id = pproto->payload.wm_chan.id;
					prtpe->state.wm_state.chan_freq = pproto->payload.wm_chan.freq;
				}
				
				if (pproto->type == WM_RADIO_QUALITY)
				{
					prtpe->state.wm_state.radio_quality = pproto->payload.wm_radio_quality.radio_quality;
				}
				
				if (pproto->type == WM_TRANS)
				{
					prtpe->state.wm_state.ssrc = pproto->payload.wm_trans.ssrc;
					prtpe->state.wm_state.session_addr = pproto->payload.wm_trans.session_addr;
				}
				
				if (pproto->type == WM_SEL_CALL_TRANS)
				{
					prtpe->state.wm_state.ssrc = pproto->payload.wm_sel_call_trans.ssrc;
					prtpe->state.wm_state.session_addr = pproto->payload.wm_sel_call_trans.session_addr;
					memset(prtpe->state.wm_state.callerID, 0, sizeof(prtpe->state.wm_state.callerID));
					memcpy(prtpe->state.wm_state.callerID, pproto->payload.wm_sel_call_trans.callerID, pproto->payload.wm_sel_call_trans.callerID_length);
				}
				
				if (pproto->type == WM_SELECT_RC_CHAN)
				{
					prtpe->state.wm_state.rc_interface = pproto->payload.wm_select_rc_chan.interface;
					prtpe->state.wm_state.rc_channel = pproto->payload.wm_select_rc_chan.channel;
				}
			}
		}
		
		/* process TO */
	}
	return 0;
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
/* Format str_flags : PRIORITY/T/P/R/S/B/RV/SH/PW/RS/C/STATUS */
int parse_sig_proto_flags(char *str_flags, struct sig_proto_flags	*flags)
{
	int res = 0;
	char *slash, str_value[256];
	int value;
	
	slash = str_flags;
	
	/* prio */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;
		
		flags->priority = value;		
	}
	
	/* flag_t */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;

		flags->flag_t = value;		
	}
	
	/* flag_p */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;

		flags->flag_p = value;		
	}
	
	/* flag_r */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;

		flags->flag_r = value;		
	}
	
	/* flag_s */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;

		flags->flag_s = value;		
	}

	/* flag_b */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;
		
		flags->flag_b = value;		
	}
	
	/* flag_rv */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;

		flags->flag_rv = value;		
	}
	
	/* flag_sh */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;

		flags->flag_sh = value;		
	}
	
	/* flag_pw */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;

		flags->flag_pw = value;		
	}
	
	/* flag_rs */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;

		flags->flag_rs = value;		
	}
	
	/* flag_c */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;

		flags->flag_c = value;		
	}
	
	/* status */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;

		flags->status = value;		
	}	
	
	return res;
}
/*----------------------------------------------------------------------------*/
/* Format str_adj_chan : ID/FREQ */
int parse_sig_proto_adj_chan(char *str_adj_chan, int *chan_id, float *chan_freq)
{
	int res = 0;
	char *slash, str_value[256];
	int value;
	float fvalue;
	
	slash = str_adj_chan;
	
	/* ID */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;
		
		if (chan_id)
			*chan_id = value;
	}
	
	/* FLOAT */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		fvalue = atof(str_value);
		res++;
		
		if (chan_freq)
			*chan_freq = fvalue;
	}

	return res;
}
/*----------------------------------------------------------------------------*/
/* Format str_rc_chan : INTERFACE/CHANNEL */
int parse_sig_proto_select_rc_chan(char *str_rc_chan, int *interface, int *channel)
{
	int res = 0;
	char *slash, str_value[256];
	int value;
	
	slash = str_rc_chan;
	
	/* INTERFACE */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;
		
		if (interface)
			*interface = value;
	}
	
	/* CHANNEL */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;
		
		if (channel)
			*channel = value;
	}

	return res;
}
/*----------------------------------------------------------------------------*/
int parse_sig_proto_trans(char *str_rc_chan, struct in_addr	*session_addr, unsigned long *ssrc, char *callerID, int callerID_size)
{
	int res = 0;
	char *slash, str_value[256];
	int value;
	
	slash = str_rc_chan;
	
	/* session_addr */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		res++;
		
		if (session_addr)
			inet_pton(AF_INET, str_value, session_addr);
	}
	
	/* ssrc */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		value = atoi(str_value);
		res++;
		
		if (ssrc)
			*ssrc = value;
	}
	
	/* callerID */
	if (get_next_param(&slash, str_value, sizeof(str_value)))
	{
		res++;
		
		if (callerID && (callerID_size > 0))
		{
			memset(callerID, 0, callerID_size);
			memcpy(callerID, str_value, callerID_size-1);
		}
	}

	return res;
}
/*----------------------------------------------------------------------------*/

