#ifndef __RTP_EXT_H
#define __RTP_EXT_H

#include "voice-ioctl.h"
#include "cb.h"

#define DATA_BUFFER_LENGTH				1024
#define RTP_EXT_PAYLOAD_MAX_SIZE		64
#define RTP_EXT_ADDR_STRUCT_SIZE		12

struct session;

typedef struct session_ext_stats
{
	u32		ptx;
	u32		prx;

} session_ext_stats_t;

typedef struct session_ext_addr
{
	u32				ssrc;
	struct in_addr	src_addr;
	u16				src_port;
	u16				etype;

} session_ext_addr_t;

typedef struct session_ext
{
	struct rtp_ext_connection_params parm;

	/* attached rtp session */
	struct session				*rtp_session;
	wait_queue_head_t 			pwait;
	
	struct list_head 			node;	
	struct s_circular_buffer	tx_buf;
	struct s_circular_buffer	rx_buf;
	char						int_tx_payload[RTP_EXT_PAYLOAD_MAX_SIZE];
	char						int_rx_payload[RTP_EXT_PAYLOAD_MAX_SIZE+RTP_EXT_ADDR_STRUCT_SIZE];
	char						int_unpack_rx_payload[RTP_EXT_PAYLOAD_MAX_SIZE+RTP_EXT_ADDR_STRUCT_SIZE];
	struct session_ext_addr		int_unpack_rx_addr;
	
	char						skt_tx_payload[RTP_EXT_PAYLOAD_MAX_SIZE];
	struct session_ext_addr		skt_pack_rx_addr;
	char						skt_pack_rx_payload[RTP_EXT_PAYLOAD_MAX_SIZE+RTP_EXT_ADDR_STRUCT_SIZE];	
	int							signal_rx;

	/* statistics */
	char						active;
	session_ext_stats_t			stat;
} session_ext_t;

int rtp_ext_signal_rx(session_ext_t *s);
size_t rtp_ext_pack_rx(session_ext_t *s, char *buf, size_t count);

#endif /* __RTP_EXT_H */


