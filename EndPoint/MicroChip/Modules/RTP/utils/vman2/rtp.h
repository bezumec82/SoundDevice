#ifndef _RTP_H_
#define _RTP_H_

#include "voice-ioctl.h"
#include "hal-ioctl.h"
#include "list.h"

#define MAX_STR	255


struct rtp_session_params {
	char				dest[MAX_STR];
	int		 			channel;
	char				payload[MAX_STR];
	unsigned			pf;
	char				attr[MAX_STR];
};

struct rtp_session
{
	struct list_head				list;
	
	/* config */
	struct rtp_session_params		params;
	/* state */
	int								h_rtp;
	int								h_ctl;
	int								h_conf;
	int								h_dtmf;
    int								hal_ctl;

	struct rtp_connection_params	conn_param;
	struct rtp_filter_params		filter_param;
	struct rtp_stat_info			stat_info;
	int								chnid;
	struct rtp_channel_info			chn_info;
	struct mix_slot					slot;
	struct mix_slot_pair			slot_pair;
	int								tdm;
	int								channel;
};

int rtp_session_open(struct rtp_session *prtp, struct rtp_session_params *pparam);
int rtp_session_close(struct rtp_session *prtp);
int rtp_session_fshow(FILE * stream, struct rtp_session *prtp);
int rtp_session_filter_set(struct rtp_session *prtp, struct rtp_filter_params	*parm);
int parse_filter_param(char *str_filter, struct rtp_filter_params *parm);

#endif /* _RTP_H_ */


