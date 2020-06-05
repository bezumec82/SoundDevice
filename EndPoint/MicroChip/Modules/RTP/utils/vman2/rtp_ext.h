#ifndef _RTP_EXT_H_
#define _RTP_EXT_H_

#include "voice-ioctl.h"
#include "hal-ioctl.h"
#include "list.h"
#include "sig_proto.h"

struct rtp_ext_session_params
{
	int		h_rtp;
};

struct radio_rtp_wm_state
{
	unsigned long						priority;
	unsigned long 						flag_t;
	unsigned long 						flag_p;
	unsigned long 						flag_r;
	unsigned long 						flag_s;
	unsigned long 						flag_b;
	unsigned long 						flag_rv;
	unsigned long 						flag_sh;
	unsigned long 						flag_rs;
	unsigned long 						flag_pw;
	unsigned long 						flag_c;
	unsigned long						status;
	
	float								radio_quality;
	
    unsigned long						chan_id;
    float								chan_freq;	
	
	unsigned char						rc_interface;
	unsigned char						rc_channel;
	
	struct 	in_addr						session_addr;
	unsigned long						ssrc;
	char 								callerID[CALLERID_LEN];
};

struct radio_rtp_gw_state
{
	unsigned long						priority;
	unsigned long 						flag_t;
	unsigned long 						flag_p;
	unsigned long 						flag_r;
	unsigned long 						flag_s;
	unsigned long 						flag_b;
	unsigned long 						flag_rv;
	unsigned long 						flag_sh;
	unsigned long 						flag_rs;
	unsigned long 						flag_pw;
	unsigned long 						flag_c;
	unsigned long						status;
	
    unsigned long						chan_id;
    float								chan_freq;	
	
	unsigned char						rc_interface;
	unsigned char						rc_channel;

	char 								callerID[CALLERID_LEN];
};


struct radio_rtp_state
{
	unsigned int						packets_send_count[256];
	unsigned int						packets_recv_count[256];
	
	struct 	in_addr						rtp_session_addr;
	unsigned long						rtp_ssrc;
	
	struct radio_rtp_wm_state			wm_state;
	struct radio_rtp_gw_state			gw_state;
};

struct rtp_ext_session
{
	struct list_head					list;
	
	/* config */
	struct rtp_ext_session_params		params;
	/* state */
	int									h_rtp_ext;
	unsigned long						poll_count;
	unsigned long						total_read_ioctl_packets;
	unsigned long						total_read_packets;
	unsigned long						total_write_packets;
	unsigned long						total_read_bytes;
	unsigned long						total_write_bytes;
	int									sig_proto_tx_last_error;
	int									sig_proto_rx_last_error;
	int									last_error_ioctl;
	int									last_error_read;
	int									last_error_write;	
	char								tx_payload[256];
	char								rx_payload[256];
	struct rtp_ext_source_addr			rx_saddr;
	
	struct radio_rtp_state				state;


	struct rtp_ext_connection_params	conn_param;
	struct sig_proto					proto_tx;
	struct sig_proto					proto_rx;
};

int rtp_ext_session_open(struct rtp_ext_session *prtpe, struct rtp_ext_session_params *pparam);
int rtp_ext_session_close(struct rtp_ext_session *prtpe);
int rtp_ext_session_read(struct rtp_ext_session *prtpe);
int rtp_ext_session_process(struct rtp_ext_session *prtpe, struct sig_proto *pproto);
int rtp_ext_session_write(struct rtp_ext_session *prtpe, struct sig_proto *pproto);
int rtp_ext_session_fshow(FILE * stream, struct rtp_ext_session *prtpe);
int parse_sig_proto_flags(char *str_flags, struct sig_proto_flags	*flags);
int parse_sig_proto_adj_chan(char *str_adj_chan, int *chan_id, float *chan_freq);
int parse_sig_proto_select_rc_chan(char *str_rc_chan, int *interface, int *channel);
int parse_sig_proto_trans(char *str_rc_chan, struct in_addr	*session_addr, unsigned long *ssrc, char *callerID, int callerID_size);

#endif /* _RTP_EXT_H_ */


