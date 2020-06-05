#ifndef __RTP_H
#define __RTP_H

#include "voice-ioctl.h"
#include "rtp_api.h"
#include "cb.h"

#define _RTP_VERSION 2
#define RTP_HEADER_LENGTH   32

#define PAYLOAD_TYPE_CN     13
#define RTP_STATE_INITIAL   0x0001

struct channel;
struct session_ext;

typedef struct conn_stats
{
    u32     ptx;
    u32     prx;
    u32     tx_dropped;
    u32     rx_dropped;
    u32     tx_last;    /* jiffies */
    u32     rx_last;    /* jiffies */
#if 0   
    u32     bytes;      /* amount of bytes recieved/transmitted */
    u32     pkts_late;  /* late packets */
    u32     pkts_early; /* early packets */
    u32     pkts_lost;  /* lost packets */
    u32     pkts_oos;   /* oos packets */
    u32     min_jbuffer;
    u32     max_jbuffer;
    u32     avg_jbuffer;
    struct timeval  started_at;
#endif
} stats_t;

typedef struct session
{
    struct rtp_connection_params    parm;
    struct rtp_filter_params        filter;

    int pcm_frame_size;
    int code_frame_size;

    /* static private data */    
    struct session_ext *            rtp_ext_session;
    struct channel *                channel;
    struct socket *                 socket;
    struct list_head                node;
    wait_queue_head_t               pwait;

    /* dynamic private data */
    unsigned short                  seq_no;
    unsigned int                    time_elapsed;

    /* statistics */
    char    active_rtp_ext;
    char    active_filter;
    char    active;
    stats_t stat;
} session_t;

struct skb_internals
{
    u8  payloadtype;
    u8  marker;
    u16 seq;
    u32 ts;
    u32 local_ts;
    struct session* session;
    u32 ssrc;
    u8  late;
    long    payload_len;
}  __attribute__ ((packed));
typedef struct skb_internals skb_int_t;

#define skb_payloadtype(x)  (((skb_int_t *) ((x)->cb))->payloadtype)
#define skb_marker(x)       (((skb_int_t *) ((x)->cb))->marker)
#define skb_seq(x)          (((skb_int_t *) ((x)->cb))->seq)
#define skb_ts(x)           (((skb_int_t *) ((x)->cb))->ts)
#define skb_localts(x)      (((skb_int_t *) ((x)->cb))->local_ts)
#define skb_ssrc(x)         (((skb_int_t *) ((x)->cb))->ssrc)
#define skb_late(x)         (((skb_int_t *) ((x)->cb))->late)
#define skb_session(x)      (((skb_int_t *) ((x)->cb))->session)
#define skb_payload_len(x)      (((skb_int_t *) ((x)->cb))->payload_len)

extern int rtp_rx_callback(struct sk_buff *skb);
extern int rtp_ext_rx_callback(struct sk_buff *skb, rtp_packet *packet);
extern int rtp_rx_filter_callback(struct sk_buff *skb, rtp_packet *packet);
extern void rtp_send_skb(struct sk_buff *skb);

int rtp_create_connection(session_t*);
void rtp_close_connection(session_t*);


#endif
