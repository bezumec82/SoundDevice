#include "global.h"

int rtp_rx_filter_callback(struct sk_buff *skb, rtp_packet *packet)
{
	int accept = 1;
	session_t* s = skb_session(skb);
	struct iphdr* iph;
	if (s && s->active_filter)
	{
		iph = ip_hdr(skb);
		accept = 	(s->filter.recv_addr == 0) || 
					(iph->saddr == 0) || 
					(iph->saddr == s->filter.recv_addr);
		if (packet)
		{
			accept &=	(s->filter.ssrc == 0) || 
						(packet->RTP_header->ssrc == 0) ||
						(s->filter.ssrc == packet->RTP_header->ssrc);
		}
	}
	return accept;
}
/*--- End of function ------------------------------------------------------*/
int rtp_ext_rx_callback(struct sk_buff *skb, rtp_packet *packet)
{
	session_t* s;
	session_ext_t* s_ext;
	int res;
	size_t pack_res, pack_size;
	struct udphdr* udph;
	struct iphdr* iph;	
	
	s = skb_session(skb);		
	if (s && s->active_rtp_ext && s->rtp_ext_session && packet && packet->RTP_extension)
	{
		iph		= ip_hdr(skb);
		udph	= udp_hdr(skb);
		s_ext = s->rtp_ext_session;
		if (s_ext->parm.etype == packet->RTP_extension->etype)
		{
			s_ext->skt_pack_rx_addr.etype			= s_ext->parm.etype;
			s_ext->skt_pack_rx_addr.ssrc			= packet->RTP_header->ssrc;
			s_ext->skt_pack_rx_addr.src_port		= udph->source;
			s_ext->skt_pack_rx_addr.src_addr.s_addr	= iph->saddr;
			
			pack_size = (sizeof(s_ext->skt_pack_rx_addr) + packet->RTP_extension->elen*4);
			pack_res = rtp_ext_pack_rx(s_ext, packet->RTP_extension->epayload, packet->RTP_extension->elen*4);
			if (pack_size != pack_res)
			{
				DPRINTK("RTP_EXT:pack_rx error, need size = %zu, result size = %zu\n", pack_size, pack_res);
			}
			else
			{
				res = circular_buffer_push(&s_ext->rx_buf, s_ext->skt_pack_rx_payload, pack_size);
				if (res < 0)
				{
					DPRINTK("RTP_EXT:circular_buffer_push rx_buf error\n");
				}
				else
				{
					rtp_ext_signal_rx(s_ext);
					s_ext->stat.prx++;
				}
			}
		}
	}
	return 0;
}
/*--- End of function ------------------------------------------------------*/
int rtp_rx_callback(struct sk_buff *skb)
{
	channel_t* c;
	session_t* s;
	s = skb_session(skb);
	s->stat.prx++;
	s->stat.rx_last = jiffies;

	c = s->channel;
	if (c) {
#if 0
		c->play_frame(c, skb);
#else
		c->write_dec_frame(c, skb_payloadtype(skb), skb->data, skb->len-skb_payload_len(skb));
		dev_kfree_skb(skb);
#endif
	}
	else
	{
		dev_kfree_skb(skb);
	}
	return 0;
}
/*--- End of function --------------------------------*/
