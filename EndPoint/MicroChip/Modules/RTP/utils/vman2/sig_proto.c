#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "sig_proto.h"



long sprintf_base(char *buff,long size, struct sig_proto *sp, int mode);
long sprintf_gw_adjust_chan(char *buff,long size, struct sig_proto *sp, int mode);
long sprintf_gw_selcall(char *buff,long size, struct sig_proto *sp, int mode);
long sprintf_gw_select_rc_chan(char *buff,long size, struct sig_proto *sp, int mode);

long sprintf_wm_trans(char *buff,long size, struct sig_proto *sp, int mode);
long sprintf_wm_radio_qulity(char *buff,long size, struct sig_proto *sp, int mode);
long sprintf_wm_chan(char *buff,long size, struct sig_proto *sp, int mode);
long sprintf_wm_selcall(char *buff,long size, struct sig_proto *sp, int mode);
long sprintf_wm_select_rc_chan(char *buff,long size, struct sig_proto *sp, int mode);

static struct s_sig_proto_sprintf sig_proto_sprintf_table[] =
{
	{0,					sprintf_base},
	
	{GW_FLAG,			sprintf_base},
    {GW_ADJUST_CHAN,	sprintf_gw_adjust_chan},
	{GW_FLAG_PTT,		sprintf_base},
	{GW_FLAG_BSS,		sprintf_base},
	{GW_FLAG_RV,		sprintf_base},
	{GW_FLAG_SH,		sprintf_base},
	{GW_FLAG_PW,		sprintf_base},
	{GW_FLAG_RS,		sprintf_base},
	{GW_SEL_CALL,		sprintf_gw_selcall},
	{GW_SELECT_RC_CHAN, sprintf_gw_select_rc_chan},

	{WM_FLAG,			sprintf_base},
	{WM_TRANS,			sprintf_wm_trans},
    {WM_RADIO_QUALITY,	sprintf_wm_radio_qulity},
    {WM_CHAN,			sprintf_wm_chan},
    {WM_SEL_CALL_TRANS,	sprintf_wm_selcall},
    {WM_SELECT_RC_CHAN,	sprintf_wm_select_rc_chan}
};
 
struct s_sig_proto_table_name sig_proto_packet_const_name[SIG_PROTO_TABLE_NAME_LENGHT]={
	
	{GW_FLAG,			"GW_FLAG"},
    {GW_ADJUST_CHAN,	"GW_ADJUST_CHAN"},
	{GW_FLAG_PTT,		"GW_FLAG_PTT"},
	{GW_FLAG_BSS,		"GW_FLAG_BSS"},
	{GW_FLAG_RV,		"GW_FLAG_RV"},
	{GW_FLAG_SH,		"GW_FLAG_SH"},
	{GW_FLAG_PW,		"GW_FLAG_PW"},
	{GW_FLAG_RS,		"GW_FLAG_RS"},
	{GW_SEL_CALL,		"GW_SEL_CALL"},
	{GW_SELECT_RC_CHAN, "GW_SELECT_RC_CHAN"},

	{WM_FLAG,			"WM_FLAG"},
	{WM_TRANS,			"WM_TRANS"},
    {WM_RADIO_QUALITY,	"WM_RADIO_QUALITY"},
    {WM_CHAN,			"WM_CHAN"},
    {WM_SEL_CALL_TRANS,	"WM_SEL_CALL_TRANS"},
    {WM_SELECT_RC_CHAN,	"WM_SELECT_RC_CHAN"},
    	
    {-1,				NULL},
}; 
/*----------------------------------------------------------------------------*/
char * sig_proto_find_name_by_id(struct s_sig_proto_table_name *table, int id)
{
	char *res = NULL;
	int i;
	if (table != NULL)
	for (i=0; (i<SIG_PROTO_TABLE_NAME_LENGHT) && (res == NULL) && (table[i].name != NULL); i++)
	{
		if (table[i].id == id) res = table[i].name;
	}	
	
	return res;
}
/*----------------------------------------------------------------------------*/
int sig_proto_find_id_by_name(struct s_sig_proto_table_name *table, char *name)
{
	int res = -1, find = -1;
	int i;
	if ((table != NULL) && (name != NULL))
	for (i=0; (i<SIG_PROTO_TABLE_NAME_LENGHT) && (find < 0) && (table[i].name != NULL); i++)
	{		
		if (strcmp(table[i].name, name) == 0) 
		{
			find = i;
			res = table[i].id;
		}
	}	
	
	return res;	
}
/* -------------------------------------------------------------------------- */
long cmp_sig_proto(struct sig_proto *d1, struct sig_proto *d2)
{
	long res = -1;
	char buff_data1[8192];
	char buff_data2[8192];
	long size_data1;
	long size_data2;
	
	if (d1 && d2)
	{
		size_data1 = sprintf_sig_proto(buff_data1, sizeof(buff_data1), d1, 0);
		size_data2 = sprintf_sig_proto(buff_data2, sizeof(buff_data2), d2, 0);
		
		res = (size_data2 - size_data1);
		if (!res)
		{
			res = strncmp(buff_data1, buff_data2, size_data1);
		}
	}
	return res;
}
/*----------------------------------------------------------------------------*/
long sprintf_sig_proto(char *buff, long size, struct sig_proto *sp, int mode)
{
	long curr_size = 0;
	struct s_sig_proto_sprintf *pps;
	int i, pps_len;
	
	if (buff && sp && (size > 0))
	{
		memset(buff, 0, size);
		pps_len = sizeof(sig_proto_sprintf_table) / sizeof(sig_proto_sprintf_table[0]);
		pps = NULL;
		for (i = 0;(i < pps_len) && (pps == NULL);i++)
		{
			if (sp->type == sig_proto_sprintf_table[i].type)
			{
				pps = &sig_proto_sprintf_table[i];
			}
		}

		if (pps && pps->sprintf)
		{
			curr_size = pps->sprintf(buff, size, sp, mode);
		}
	}	
	return curr_size;
}
/*----------------------------------------------------------------------------*/
long sprintf_base(char *buff,long size, struct sig_proto *sp, int mode)
{
	char *msgName;
	long curr_size = 0;
	if (buff && sp && (size > 0))
	{
		memset(buff, 0, size--);
		
		if (mode & SIG_PROTO_SPRINTF_MODE_PRETTY)
		{
			msgName = sig_proto_find_name_by_id(sig_proto_packet_const_name, sp->type);
			if (msgName)
			{
				curr_size += snprintf(&buff[curr_size], size-curr_size, "type      : %s,\n", msgName);
			}
			else
			{
				curr_size += snprintf(&buff[curr_size], size-curr_size, "type      : 0x%08lX,\n", sp->type);
			}
		}
		else
		{
			curr_size += snprintf(&buff[curr_size], size-curr_size, "type      : 0x%08lX,\n", sp->type);
		}
		
		curr_size += snprintf(&buff[curr_size], size-curr_size, "priority : %ld,\n", sp->flags.priority);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "flag_t   : %ld,\n", sp->flags.flag_t);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "flag_p   : %ld,\n", sp->flags.flag_p);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "flag_r   : %ld,\n", sp->flags.flag_r);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "flag_s   : %ld,\n", sp->flags.flag_s);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "flag_b   : %ld,\n", sp->flags.flag_b);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "flag_rv  : %ld,\n", sp->flags.flag_rv);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "flag_sh  : %ld,\n", sp->flags.flag_sh);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "flag_rs  : %ld,\n", sp->flags.flag_rs);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "flag_pw  : %ld,\n", sp->flags.flag_pw);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "flag_c   : %ld,\n", sp->flags.flag_c);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "status   : %ld,\n", sp->flags.status);
	}
	return curr_size;
}
/* -------------------------------------------------------------------------- */
long sprintf_gw_adjust_chan(char *buff,long size, struct sig_proto *sp, int mode)
{
	long curr_size = 0;
	if (buff && sp && (size > 0))
	{
		memset(buff, 0, size);		
		curr_size = sprintf_base(buff, size, sp, mode);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "gw_adjust_chan.id   : %ld,\n", sp->payload.gw_adjust_chan.id);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "gw_adjust_chan.freq : %4.4f,\n", sp->payload.gw_adjust_chan.freq);
	}
	return curr_size;
}
/* -------------------------------------------------------------------------- */
long sprintf_gw_selcall(char *buff,long size, struct sig_proto *sp, int mode)
{
	int i;
	long curr_size = 0;
	if (buff && sp && (size > 0))
	{
		memset(buff, 0, size);		
		curr_size = sprintf_base(buff, size, sp, mode);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "gw_sel_call.callerID_length: %d,\n", sp->payload.gw_sel_call.callerID_length);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "gw_sel_call.callerID : \n");
		for (i=0;i<sp->payload.gw_sel_call.callerID_length;i++)
		{
			curr_size += snprintf(&buff[curr_size], size-curr_size, "[%d] : 0x%02X,\n", i, sp->payload.gw_sel_call.callerID[i]);
		}
	}
	return curr_size;
}
/* -------------------------------------------------------------------------- */
long sprintf_gw_select_rc_chan(char *buff,long size, struct sig_proto *sp, int mode)
{
	long curr_size = 0;
	if (buff && sp && (size > 0))
	{
		memset(buff, 0, size);		
		curr_size = sprintf_base(buff, size, sp, mode);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "gw_select_rc_chan.interface : %d,\n", sp->payload.gw_select_rc_chan.interface);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "gw_select_rc_chan.channel   : %d,\n", sp->payload.gw_select_rc_chan.channel);
	}
	return curr_size;
}
/* -------------------------------------------------------------------------- */
long sprintf_wm_trans(char *buff,long size, struct sig_proto *sp, int mode)
{
	long curr_size = 0;
	char str_session_addr[32];
	if (buff && sp && (size > 0))
	{
		memset(buff, 0, size);		
		curr_size = sprintf_base(buff, size, sp, mode);
		memset(str_session_addr, 0, sizeof(str_session_addr));
		inet_ntop(AF_INET, &sp->payload.wm_trans.session_addr, str_session_addr, sizeof(str_session_addr)-1);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "wm_trans.session_addr : %s,\n", str_session_addr);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "wm_trans.ssrc         : 0x%08lX,\n", sp->payload.wm_trans.ssrc);
	}
	return curr_size;	
}
/* -------------------------------------------------------------------------- */
long sprintf_wm_radio_qulity(char *buff,long size, struct sig_proto *sp, int mode)
{
	long curr_size = 0;
	if (buff && sp && (size > 0))
	{
		memset(buff, 0, size);		
		curr_size = sprintf_base(buff, size, sp, mode);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "wm_radio_quality.radio_quality : %4.5f,\n", sp->payload.wm_radio_quality.radio_quality);
	}
	return curr_size;	
}
/* -------------------------------------------------------------------------- */
long sprintf_wm_chan(char *buff,long size, struct sig_proto *sp, int mode)
{
	long curr_size = 0;
	if (buff && sp && (size > 0))
	{
		memset(buff, 0, size);		
		curr_size = sprintf_base(buff, size, sp, mode);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "wm_chan.id   : %ld,\n", sp->payload.wm_chan.id);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "wm_chan.freq : %4.4f,\n", sp->payload.wm_chan.freq);
	}
	return curr_size;	
}
/* -------------------------------------------------------------------------- */
long sprintf_wm_selcall(char *buff,long size, struct sig_proto *sp, int mode)
{
	int i;
	long curr_size = 0;
	char str_session_addr[32];
	if (buff && sp && (size > 0))
	{
		memset(buff, 0, size);		
		curr_size = sprintf_base(buff, size, sp, mode);
		memset(str_session_addr, 0, sizeof(str_session_addr));
		inet_ntop(AF_INET, &sp->payload.wm_sel_call_trans.session_addr, str_session_addr, sizeof(str_session_addr)-1);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "wm_sel_call_trans.session_addr : %s,\n", str_session_addr);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "wm_sel_call_trans.ssrc         : 0x%08lX,\n", sp->payload.wm_sel_call_trans.ssrc);
		
		curr_size += snprintf(&buff[curr_size], size-curr_size, "wm_sel_call_trans.callerID_length: %d,\n", sp->payload.wm_sel_call_trans.callerID_length);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "wm_sel_call_trans.callerID : \n");
		for (i=0;i<sp->payload.wm_sel_call_trans.callerID_length;i++)
		{
			curr_size += snprintf(&buff[curr_size], size-curr_size, "[%d] : 0x%02X,\n", i, sp->payload.wm_sel_call_trans.callerID[i]);
		}
	}
	return curr_size;	
}
/* -------------------------------------------------------------------------- */
long sprintf_wm_select_rc_chan(char *buff,long size, struct sig_proto *sp, int mode)
{
	long curr_size = 0;
	if (buff && sp && (size > 0))
	{
		memset(buff, 0, size);		
		curr_size = sprintf_base(buff, size, sp, mode);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "wm_select_rc_chan.interface : %d,\n", sp->payload.wm_select_rc_chan.interface);
		curr_size += snprintf(&buff[curr_size], size-curr_size, "wm_select_rc_chan.channel   : %d,\n", sp->payload.wm_select_rc_chan.channel);
	}
	return curr_size;	
}
/* -------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------- */
int 	sig_proto_init(struct sig_proto *sp)
{
	if (sp)
		memset(sp, 0, sizeof(*sp));
	return 0;
}
/* -------------------------------------------------------------------------- */
int 	sig_proto_set_base(struct sig_proto *sp, 
							int priority, 
							int flag_t, int flag_p,
							int flag_r, int flag_s, 
							int flag_b, int flag_rv, 
							int flag_sh, 
							int flag_pw, int flag_rs,
							int flag_c,
							int status)
{
	if (sp)
	{
		sp->flags.priority	= (priority < 0) ? sp->flags.priority : priority;
		sp->flags.flag_t 	= (flag_t < 0) ? sp->flags.flag_t : flag_t;
		sp->flags.flag_p 	= (flag_p < 0) ? sp->flags.flag_p : flag_p;
		sp->flags.flag_r 	= (flag_r < 0) ? sp->flags.flag_r : flag_r;
		sp->flags.flag_s 	= (flag_s < 0) ? sp->flags.flag_s : flag_s;
		sp->flags.flag_b 	= (flag_b < 0) ? sp->flags.flag_b : flag_b;
		sp->flags.flag_rv	= (flag_rv < 0) ? sp->flags.flag_rv : flag_rv;
		sp->flags.flag_sh	= (flag_sh < 0) ? sp->flags.flag_sh : flag_sh;
		sp->flags.flag_rs	= (flag_rs < 0) ? sp->flags.flag_rs : flag_rs;
		sp->flags.flag_pw	= (flag_pw < 0) ? sp->flags.flag_pw : flag_pw;
		sp->flags.flag_c	= (flag_c < 0) ? sp->flags.flag_c : flag_c;
		sp->flags.status 	= (status < 0) ? sp->flags.status : status;
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int 	sig_proto_set_gw_adjust_chan(struct sig_proto *sp, unsigned int	id, float	freq)
{
    if (sp)
    {
        sp->payload.gw_adjust_chan.id 		= id;
        sp->payload.gw_adjust_chan.freq 	= freq;
    }
    return 0;
}
/* -------------------------------------------------------------------------- */
int sig_proto_set_gw_selcall(struct sig_proto *sp, const char *callerID, unsigned char callerID_length)
{
	if (sp)
	{
		if (callerID)
		{
			memset(sp->payload.wm_sel_call_trans.callerID, 0,
					sizeof(sp->payload.wm_sel_call_trans.callerID));
			memcpy(sp->payload.wm_sel_call_trans.callerID, 
					callerID, 
					sizeof(sp->payload.wm_sel_call_trans.callerID)-1);
			sp->payload.wm_sel_call_trans.callerID_length = callerID_length;
		}
		else
		{
			sp->payload.wm_sel_call_trans.callerID[0]=0;
			sp->payload.wm_sel_call_trans.callerID_length=0;			
		}		
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int sig_proto_set_gw_select_rc_chan(struct sig_proto *sp, unsigned char interface, unsigned char channel)
{
	if (sp)
	{
		sp->payload.gw_select_rc_chan.interface	= interface;
		sp->payload.gw_select_rc_chan.channel	= channel;
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int 	sig_proto_set_wm_radio_quality(struct sig_proto *sp, float	radio_quality)
{
	if (sp)
	{
		sp->payload.wm_radio_quality.radio_quality = radio_quality;
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int 	sig_proto_set_wm_transmite(struct sig_proto *sp, struct in_addr session_addr, unsigned int	ssrc)
{
	if (sp)
	{
		sp->payload.wm_trans.session_addr 	= session_addr;
		sp->payload.wm_trans.ssrc 			= ssrc;
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int sig_proto_set_wm_selcall_transmite(struct sig_proto *sp, struct in_addr session_addr, unsigned int	ssrc, const char *callerID, unsigned char callerID_length)
{
	if (sp)
	{
		sp->payload.wm_sel_call_trans.session_addr 	= session_addr;
		sp->payload.wm_sel_call_trans.ssrc 			= ssrc;
		if (callerID)
		{
			memset(sp->payload.wm_sel_call_trans.callerID, 0,
					sizeof(sp->payload.wm_sel_call_trans.callerID));
			memcpy(sp->payload.wm_sel_call_trans.callerID, 
					callerID, 
					sizeof(sp->payload.wm_sel_call_trans.callerID)-1);
			sp->payload.wm_sel_call_trans.callerID_length = callerID_length;
		}
		else
		{
			sp->payload.wm_sel_call_trans.callerID[0]=0;
			sp->payload.wm_sel_call_trans.callerID_length=0;			
		}		
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int sig_proto_set_wm_select_rc_chan(struct sig_proto *sp, unsigned char interface, unsigned char channel)
{
	if (sp)
	{
		sp->payload.wm_select_rc_chan.interface	= interface;
		sp->payload.wm_select_rc_chan.channel	= channel;
	}
	return 0;
}
/* -------------------------------------------------------------------------- */
int 	sig_proto_set_wm_chan(struct sig_proto *sp, unsigned int	id, float	freq)
{
    if (sp)
    {
        sp->payload.wm_chan.id = id;
        sp->payload.wm_chan.freq = freq;
    }
    return 0;
}
/* -------------------------------------------------------------------------- */
int sig_proto_is_ptt_command(struct sig_proto *sp)
{
	int res = 0;
	if (sp)
	{
		res = 	(sp->type == GW_FLAG) ||
				(sp->type == GW_FLAG_PTT) ||
				(sp->type == GW_SEL_CALL) ||
				(sp->type == WM_FLAG) ||
				(sp->type == WM_TRANS) ||
				(sp->type == WM_SEL_CALL_TRANS);
	}
	return res;
}
/* -------------------------------------------------------------------------- */
int sig_proto_unpack(struct sig_proto *sp, unsigned char 	*payload_ext, int payload_len)
{
	unsigned char 	*protoheader;
	int 			protolen=0, needlen;
	
	unsigned long	type;
	unsigned long	priority;
	unsigned long	flag_t, flag_p, flag_r, flag_s, flag_b, flag_rv, flag_sh, flag_rs, flag_pw, flag_c;
	unsigned long	status;
	unsigned long	value;
	int 			i, callerID_length;
	
	union sig_proto_payload	payload;
	union u32float	conv_uni;
		
	int res = SIG_PROTO_UNPACK_NULL_PTR;
    if ((sp != NULL) && (payload_ext != NULL))
	{
		needlen = 4;
		if (payload_len >= needlen)
		{
			protoheader = payload_ext;
			
			type 		= protoheader[protolen++];
			priority	= SHRM(protoheader[protolen], 0x0F, 4);
			flag_t		= SHRM(protoheader[protolen], 0x01, 3);
			flag_p		= SHRM(protoheader[protolen], 0x01, 2);
			flag_r		= SHRM(protoheader[protolen], 0x01, 1);
			flag_s		= SHRM(protoheader[protolen], 0x01, 0);
			protolen++;			
			flag_b		= SHRM(protoheader[protolen], 0x01, 7);
			flag_rv		= SHRM(protoheader[protolen], 0x01, 6);
			flag_sh		= SHRM(protoheader[protolen], 0x01, 5);
			flag_pw		= SHRM(protoheader[protolen], 0x01, 4);
			status		= SHRM(protoheader[protolen], 0x0F, 0);
			protolen++;
			value		= SHRM(protoheader[protolen], 0x1F, 3);
			status 		= ASHL8(status, 5, value);
			flag_c		= SHRM(protoheader[protolen], 0x03, 1);
			flag_rs		= SHRM(protoheader[protolen], 0x01, 0);
			protolen++;

			payload			= sp->payload;

			res = SIG_PROTO_UNPACK_OK;
			switch (type)
			{
				case GW_FLAG:
				case GW_FLAG_PTT:
				case GW_FLAG_BSS:
				case GW_FLAG_RV:
				case GW_FLAG_SH:
				case GW_FLAG_PW:
				case GW_FLAG_RS:
				break;
				case GW_ADJUST_CHAN:
					needlen += 8;
					if (payload_len >= needlen)
					{
						value = ASHL8(0, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);

						payload.gw_adjust_chan.id = value;

						value = ASHL8(0, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						
						conv_uni.u32 = value;
						payload.gw_adjust_chan.freq = conv_uni.flt;
					}
					else
					{
						res = SIG_PROTO_UNPACK_BAD_LENGHT;	
					}
				break;
				case GW_SEL_CALL:
					needlen += 1;
					if (payload_len >= needlen)
					{
						callerID_length = protoheader[protolen++];
						needlen += callerID_length;
						if (payload_len >= needlen)
						{
							payload.gw_sel_call.callerID_length = callerID_length;
							for (i=0;i<callerID_length;i++)
							{
								payload.gw_sel_call.callerID[i] = protoheader[protolen++];
							}
						}
						else
						{
							res = 	SIG_PROTO_UNPACK_BAD_LENGHT;
						}
					}
					else
					{
						res = 	SIG_PROTO_UNPACK_BAD_LENGHT;
					}
				break;
				case GW_SELECT_RC_CHAN:
					needlen += 4;
					if (payload_len >= needlen)
					{
						payload.gw_select_rc_chan.interface = protoheader[protolen++];
						payload.gw_select_rc_chan.channel = protoheader[protolen++];
					}
					else
					{
						res = 	SIG_PROTO_UNPACK_BAD_LENGHT;
					}
				break;				
				case WM_FLAG:
				break;
				case WM_TRANS:
				
					needlen += 8;
					if (payload_len >= needlen)
					{
						value = ASHL8(0, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);

						payload.wm_trans.session_addr.s_addr = value;

						value = ASHL8(0, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);

						payload.wm_trans.ssrc = value;
					}
					else
					{
						res = SIG_PROTO_UNPACK_BAD_LENGHT;	
					}
				break;
				case WM_RADIO_QUALITY:
					needlen += 4;
					if (payload_len >= needlen)
					{
						value = ASHL8(0, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);

						conv_uni.u32 = value;
						payload.wm_radio_quality.radio_quality = conv_uni.flt;
					}
					else
					{
						res = SIG_PROTO_UNPACK_BAD_LENGHT;	
					}
				break;
				case WM_CHAN:
					needlen += 8;
					if (payload_len >= needlen)
					{
						value = ASHL8(0, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);

						payload.wm_chan.id = value;

						value = ASHL8(0, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);

						conv_uni.u32 = value;
						payload.wm_chan.freq = conv_uni.flt;
					}
					else
					{
						res = SIG_PROTO_UNPACK_BAD_LENGHT;	
					}
				break;
    			case WM_SEL_CALL_TRANS:
					needlen += 9;
					if (payload_len >= needlen)
					{
						value = ASHL8(0, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);

						payload.wm_sel_call_trans.session_addr.s_addr = value;

						value = ASHL8(0, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);
						value = ASHL8(value, 8, protoheader[protolen++]);

						payload.wm_sel_call_trans.ssrc = value;
						
						callerID_length = protoheader[protolen++];
						needlen += callerID_length;
						if (payload_len >= needlen)
						{
							payload.wm_sel_call_trans.callerID_length = callerID_length;
							for (i=0;i<callerID_length; i++)
							{
								payload.wm_sel_call_trans.callerID[i] = protoheader[protolen++];
							}
						}
						else
						{
							res = SIG_PROTO_UNPACK_BAD_LENGHT;	
						}
					}
					else
					{
						res = SIG_PROTO_UNPACK_BAD_LENGHT;	
					}    				
    			break;
    			case WM_SELECT_RC_CHAN:
					needlen += 4;
					if (payload_len >= needlen)
					{
						payload.wm_select_rc_chan.interface = protoheader[protolen++];
						payload.wm_select_rc_chan.channel = protoheader[protolen++];
					}
					else
					{
						res = 	SIG_PROTO_UNPACK_BAD_LENGHT;
					}    				
    			break;
				default:
				{
					sp->type = type;
					res = SIG_PROTO_UNPACK_BAD_TYPE;
				}
			}

			if (res == SIG_PROTO_UNPACK_OK)
			{
				if (protolen <= payload_len)
				{
					res = SIG_PROTO_UNPACK_OK;

					sp->type 		= type;
					sp->flags.priority	 	= priority;
					sp->flags.flag_p 		= flag_p;
					sp->flags.flag_r 		= flag_r;
					sp->flags.flag_s 		= flag_s;
					sp->flags.flag_t 		= flag_t;
					sp->flags.flag_b 		= flag_b;
					sp->flags.flag_rv		= flag_rv;
					sp->flags.flag_sh		= flag_sh;
					sp->flags.flag_rs		= flag_rs;
					sp->flags.flag_pw		= flag_pw;
					sp->flags.flag_c		= flag_c;
					sp->flags.status		= status;

					sp->payload		= payload;
				}
				else
				{
					res = SIG_PROTO_UNPACK_BAD_LENGHT;
				}
			}
		}
		else
		{
			res = SIG_PROTO_UNPACK_BAD_LENGHT;
		}
    }
	return  res;
}
/* -------------------------------------------------------------------------- */
int sig_proto_pack(struct sig_proto *sp, unsigned char 	*payload_ext, int payload_len)
{	
	unsigned char 	*protoheader;
	int 			protolen=0, needlen;
	int 			i;
	unsigned long	value;
	union u32float	conv_uni;
	int res = SIG_PROTO_PACK_OK;
	
    if ((sp != NULL) && (payload_ext != NULL))
	{
		needlen = 4;
		if (payload_len >= needlen)
		{
			memset(payload_ext, 0, payload_len);
			protoheader = payload_ext;
			
			protoheader[protolen++] = sp->type;
			protoheader[protolen++] = (MSHL(sp->flags.priority,0x0F,4) | MSHL(sp->flags.flag_t,0x01,3) | MSHL(sp->flags.flag_p,0x01,2) | 
								MSHL(sp->flags.flag_r,0x01,1) | MSHL(sp->flags.flag_s,0x01,0));
			
			value = SHRM(sp->flags.status,0x0F,5);
			protoheader[protolen++] = (MSHL(sp->flags.flag_b,0x01,7) | MSHL(sp->flags.flag_rv,0x01,6) | MSHL(sp->flags.flag_sh,0x01,5) | MSHL(sp->flags.flag_pw,0x01,4) | MSHL(value,0x0F,0));
			value = SHRM(sp->flags.status,0x1F,0);
			protoheader[protolen++] = (MSHL(value,0x1F,3) | MSHL(sp->flags.flag_c,0x03,1) | MSHL(sp->flags.flag_rs,0x01,0));
			
			/* protoheader[protolen++] = SHRM(sp->reserved1, 0xFF, 0); */
			
			switch ((int)sp->type)
			{
				case GW_FLAG:
				case GW_FLAG_PTT:
				case GW_FLAG_BSS:
				case GW_FLAG_RV:
				case GW_FLAG_SH:
				case GW_FLAG_PW:
				case GW_FLAG_RS:			
				break;

				case GW_ADJUST_CHAN:
					needlen += 8;
					if (payload_len >= needlen)
					{
						protoheader[protolen++] = SHRM(sp->payload.gw_adjust_chan.id, 0xFF, 24);
						protoheader[protolen++] = SHRM(sp->payload.gw_adjust_chan.id, 0xFF, 16);
						protoheader[protolen++] = SHRM(sp->payload.gw_adjust_chan.id, 0xFF, 8);
						protoheader[protolen++] = SHRM(sp->payload.gw_adjust_chan.id, 0xFF, 0);

						conv_uni.flt = sp->payload.gw_adjust_chan.freq;
						value = conv_uni.u32;
						protoheader[protolen++] = SHRM(value, 0xFF, 24);
						protoheader[protolen++] = SHRM(value, 0xFF, 16);
						protoheader[protolen++] = SHRM(value, 0xFF, 8);
						protoheader[protolen++] = SHRM(value, 0xFF, 0);
					}
					else
					{
						res = 	SIG_PROTO_PACK_BAD_LENGHT;
					}
				break;            
				case GW_SEL_CALL:
					needlen += sp->payload.gw_sel_call.callerID_length+8;
					if (payload_len >= needlen)
					{
						protoheader[protolen++] = sp->payload.gw_sel_call.callerID_length;
						for (i=0;i<sp->payload.gw_sel_call.callerID_length; i++)
						{
							protoheader[protolen++] = sp->payload.gw_sel_call.callerID[i];
						}
						/* align protolen */
						if (protolen & 0x03)
						{
							protolen += 4;
							protolen &= ~(0x03);
						}
					}
					else
					{
						res = 	SIG_PROTO_PACK_BAD_LENGHT;
					}
				break;            
				case GW_SELECT_RC_CHAN:
					needlen += 4;
					if (payload_len >= needlen)
					{
						protoheader[protolen++] = sp->payload.gw_select_rc_chan.interface;
						protoheader[protolen++] = sp->payload.gw_select_rc_chan.channel;
						protoheader[protolen++] = 0;
						protoheader[protolen++] = 0;
					}
					else
					{
						res = 	SIG_PROTO_PACK_BAD_LENGHT;
					}
				break;
				case WM_FLAG:
				break;
				case WM_TRANS:
					needlen += 8;
					if (payload_len >= needlen)
					{				
						protoheader[protolen++] = SHRM(sp->payload.wm_trans.session_addr.s_addr, 0xFF, 24);
						protoheader[protolen++] = SHRM(sp->payload.wm_trans.session_addr.s_addr, 0xFF, 16);
						protoheader[protolen++] = SHRM(sp->payload.wm_trans.session_addr.s_addr, 0xFF, 8);
						protoheader[protolen++] = SHRM(sp->payload.wm_trans.session_addr.s_addr, 0xFF, 0);
						
						protoheader[protolen++] = SHRM(sp->payload.wm_trans.ssrc, 0xFF, 24);
						protoheader[protolen++] = SHRM(sp->payload.wm_trans.ssrc, 0xFF, 16);
						protoheader[protolen++] = SHRM(sp->payload.wm_trans.ssrc, 0xFF, 8);
						protoheader[protolen++] = SHRM(sp->payload.wm_trans.ssrc, 0xFF, 0);
					}
					else
					{
						res = 	SIG_PROTO_PACK_BAD_LENGHT;
					}
				break;
				case WM_SEL_CALL_TRANS:
				
					needlen += (16+sp->payload.wm_sel_call_trans.callerID_length);
					if (payload_len >= needlen)
					{						
						protoheader[protolen++] = SHRM(sp->payload.wm_sel_call_trans.session_addr.s_addr, 0xFF, 24);
						protoheader[protolen++] = SHRM(sp->payload.wm_sel_call_trans.session_addr.s_addr, 0xFF, 16);
						protoheader[protolen++] = SHRM(sp->payload.wm_sel_call_trans.session_addr.s_addr, 0xFF, 8);
						protoheader[protolen++] = SHRM(sp->payload.wm_sel_call_trans.session_addr.s_addr, 0xFF, 0);
						
						protoheader[protolen++] = SHRM(sp->payload.wm_sel_call_trans.ssrc, 0xFF, 24);
						protoheader[protolen++] = SHRM(sp->payload.wm_sel_call_trans.ssrc, 0xFF, 16);
						protoheader[protolen++] = SHRM(sp->payload.wm_sel_call_trans.ssrc, 0xFF, 8);
						protoheader[protolen++] = SHRM(sp->payload.wm_sel_call_trans.ssrc, 0xFF, 0);
						
						protoheader[protolen++] = sp->payload.wm_sel_call_trans.callerID_length;
						for (i=0;i<sp->payload.wm_sel_call_trans.callerID_length; i++)
						{
							protoheader[protolen++] = sp->payload.wm_sel_call_trans.callerID[i];
						}
						/* align protolen */
						if (protolen & 0x03)
						{
							protolen += 4;
							protolen &= ~(0x03);
						}
					}
					else
					{
						res = 	SIG_PROTO_PACK_BAD_LENGHT;
					}
				break;
				case WM_SELECT_RC_CHAN:
					needlen += 4;
					if (payload_len >= needlen)
					{
						protoheader[protolen++] = sp->payload.wm_select_rc_chan.interface;
						protoheader[protolen++] = sp->payload.wm_select_rc_chan.channel;
						protoheader[protolen++] = 0;
						protoheader[protolen++] = 0;
					}
					else
					{
						res = 	SIG_PROTO_PACK_BAD_LENGHT;
					}
				break;
				case WM_RADIO_QUALITY:
					needlen += 4;
					if (payload_len >= needlen)
					{
						conv_uni.flt = sp->payload.wm_radio_quality.radio_quality;
						value = conv_uni.u32;
						protoheader[protolen++] = SHRM(value, 0xFF, 24);
						protoheader[protolen++] = SHRM(value, 0xFF, 16);
						protoheader[protolen++] = SHRM(value, 0xFF, 8);
						protoheader[protolen++] = SHRM(value, 0xFF, 0);
					}
					else
					{
						res = 	SIG_PROTO_PACK_BAD_LENGHT;
					}
				break;

				case WM_CHAN:
					needlen += 8;
					if (payload_len >= needlen)
					{				
						protoheader[protolen++] = SHRM(sp->payload.wm_chan.id, 0xFF, 24);
						protoheader[protolen++] = SHRM(sp->payload.wm_chan.id, 0xFF, 16);
						protoheader[protolen++] = SHRM(sp->payload.wm_chan.id, 0xFF, 8);
						protoheader[protolen++] = SHRM(sp->payload.wm_chan.id, 0xFF, 0);

						conv_uni.flt = sp->payload.wm_chan.freq;
						value = conv_uni.u32;            
						
						protoheader[protolen++] = SHRM(value, 0xFF, 24);
						protoheader[protolen++] = SHRM(value, 0xFF, 16);
						protoheader[protolen++] = SHRM(value, 0xFF, 8);
						protoheader[protolen++] = SHRM(value, 0xFF, 0);
					}
					else
					{
						res = 	SIG_PROTO_PACK_BAD_LENGHT;
					}
				break;
				default:
				{;}
			}
		}
		else
		{
			res = SIG_PROTO_PACK_BAD_LENGHT;
		}
	}
    return (res == SIG_PROTO_PACK_OK ? protolen : res);
}
/* -------------------------------------------------------------------------- */
unsigned int init_sig_proto_type[] = {
	GW_FLAG,
	GW_FLAG_PTT,
	GW_FLAG_BSS,
	GW_FLAG_RV,
	GW_FLAG_SH,
	GW_FLAG_PW,
	GW_FLAG_RS,
	GW_ADJUST_CHAN,
	GW_SEL_CALL,	
	GW_SELECT_RC_CHAN,
	
	WM_FLAG,
	WM_TRANS,
	WM_CHAN,
	WM_RADIO_QUALITY,
	WM_SEL_CALL_TRANS
};
/* -------------------------------------------------------------------------- */
int init_sig_proto_queue(struct sig_proto_queue  *queue)
{
	int i;
	int len, init_len;
	if (queue)
	{
		init_len = sizeof(init_sig_proto_type) / sizeof(init_sig_proto_type[0]);
		len = sizeof(queue->queue) / sizeof(queue->queue[0]);
		memset(queue->queue, 0, sizeof(queue->queue));
		for (i=0;i<len;i++)
		{			
			if (i < init_len)
				queue->queue[i].type = init_sig_proto_type[i];
			else
				queue->queue[i].type = -1;
		}
		
		memset(queue->need_process, 0, sizeof(queue->need_process));
	}
	return 0;	
}

/* -------------------------------------------------------------------------- */
