#include "global.h"
#include "sysdep.h"
#include "rtp.h"
#include "rtp_api.h"

static inline int RTPBuildRTPHeader(struct sk_buff *skb)
{
	/*
	 * Our packet header will be composed of 3 parts: - the header - the
	 * CSRC list - the extension It is the caller's responsibility to attach
	 * the payload and padding.
	 */
	
	int rtphdrlen = sizeof(rtp_hdr_t)-sizeof(u_int32);
	rtp_hdr_t* rh = (rtp_hdr_t*) skb_push(skb, rtphdrlen);
	session_t* s;
	session_ext_t* s_ext;
	int res;
	int rtphdrextlen, elen;
	rtp_hdr_ext* rhe;
	char *new_head;
	/* Since we just sent, update the fact that we are a sender */
	 
	SET_RTP_VERSION(*rh, _RTP_VERSION);
	SET_RTP_X(*rh, 0);
	SET_RTP_CC(*rh, 0);
	SET_RTP_M(*rh, skb_marker(skb));
	SET_RTP_PT(*rh, skb_payloadtype(skb));
	SET_RTP_P(*rh, 0);	// padding
	
	s = skb_session(skb);
	rh->seq = (u_int16) s->seq_no;
	s->seq_no++;
	s->time_elapsed += skb_ts(skb);
	rh->ts = s->time_elapsed;
	rh->ssrc = s->parm.ssrc;

	rh->seq = htons(rh->seq);
	rh->ts = htonl(rh->ts);
	rh->ssrc = htonl(rh->ssrc);
	
	if (s->active_rtp_ext && s->rtp_ext_session)
	{
		s_ext = s->rtp_ext_session;
		res = circular_buffer_pop(&s_ext->tx_buf, s_ext->skt_tx_payload, sizeof(s_ext->skt_tx_payload));
		if (res > 0)
		{
			SET_RTP_X(*rh, 1);
			elen = ((res+3)/4);
			rtphdrextlen = (elen+1)*4;
			new_head = (char *) skb_push(skb, rtphdrextlen);
			memmove(new_head, &new_head[rtphdrextlen], rtphdrlen);
			rhe = (rtp_hdr_ext*)&new_head[rtphdrlen];
			rhe->etype = s_ext->parm.etype;
			rhe->elen = elen;
			if (elen)
			{
				memset(rhe->epayload, 0, elen*4);
				memcpy(rhe->epayload, s_ext->skt_tx_payload, res);
			}
		}
		else
		{
			if (res < 0)
			{
				RPRINTK("RTP_EXT:circular_buffer_pop tx_buf error\n");
			}
		}
	}
	
	return (skb->len);
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
static inline struct sk_buff *RTPBuildHeaders(struct sk_buff *skb)
{
	session_t* s = skb_session(skb);	
	if (s->socket == NULL) return NULL;
	struct sock* sk = s->socket->sk;

	if (sk->state != TCP_ESTABLISHED) return NULL;
	if (sk->dst_cache == NULL) return NULL;
	if (sk->dst_cache->dev == NULL) return NULL;

	skb_set_owner_w(skb, sk);
//	set_bit(SOCK_ASYNC_NOSPACE, &sk->socket->flags);
    set_bit(SOCKWQ_ASYNC_NOSPACE, &sk->socket->flags);
	set_bit(SOCK_NOSPACE, &sk->socket->flags);

	skb->priority = sk->priority;
	skb->dst      = sk->dst_cache;
	skb->dev      = sk->dst_cache->dev;
	skb->protocol = __constant_htons(ETH_P_IP);

	int udppacketlen = RTPBuildRTPHeader(skb) + sizeof(struct udphdr);

	struct udphdr* udph = (struct udphdr *) skb_push(skb, sizeof(struct udphdr));
	skb->h.uh = udph;
//	udph->dest = sk->dport;
//	udph->source = sk->sport;
	udph->dest = htons(s->parm.send_port);
	udph->source = htons(s->parm.recv_port);
	udph->len = htons(udppacketlen);
	udph->check = 0;

	struct iphdr* iph = (struct iphdr *) skb_push(skb, sizeof(struct iphdr));
	skb->nh.iph = iph;
	iph->version = 4;
	iph->ihl = 5;

	iph->frag_off = __constant_htons(IP_DF);
	iph->id = sk->protinfo.af_inet.id++;
	iph->ttl = sk->protinfo.af_inet.ttl;
//	iph->daddr = sk->daddr;
	iph->daddr = s->parm.send_addr;
	iph->saddr = sk->saddr;
	iph->protocol = IPPROTO_UDP;
	iph->tot_len = htons(sizeof(struct iphdr) + udppacketlen);
	iph->tos = sk->protinfo.af_inet.tos;
	
	return skb;
}

void rtp_send_mirror_skb(struct sk_buff *skb_in)
{
	session_t* s = skb_session(skb_in);
	struct sk_buff *skb = dev_alloc_skb(1500);
	void *data;
	unsigned long copied;
	
	if (skb == NULL) return;
	skb_reserve(skb, ETH_RTP_HDR_SIZE);

	data=(void *)skb_put(skb, s->pcm_frame_size);
	copied = copy_from_user(data,skb_in->data,s->pcm_frame_size);
	skb_pull(skb_in, s->pcm_frame_size);

	skb_payloadtype(skb)=s->parm.tx_pt;
	skb_session(skb)=s;
	skb_ts(skb)=s->pcm_frame_size;
	
	if(RTPBuildHeaders(skb)) {
		ip_send_check(skb->nh.iph);
		neigh_connected_output(skb);
	}
	else dev_kfree_skb(skb);

}

void rtp_send_skb(struct sk_buff *skb)
{
	if(skb==NULL) return;

	if(RTPBuildHeaders(skb)) {
#if 0
		ip_select_ident(skb->nh.iph, skb->dst, skb->sk);
		ip_finish_output(skb);
#else		
		ip_send_check(skb->nh.iph);
		neigh_connected_output(skb);
#endif
	}
	else dev_kfree_skb(skb);
}

#else  /* 2.6.37 */

static inline struct sk_buff *RTPBuildHeaders(struct sk_buff *skb)
{
	session_t           * s = skb_session(skb);
	struct sock         * sk;
	struct inet_sock    * inet;
	struct udphdr       * udph;
	struct iphdr        * iph;
	int udppacketlen;
	
	if (s->socket == NULL) return NULL;
	sk = s->socket->sk;

	if (sk->sk_state != TCP_ESTABLISHED) return NULL;
	if (sk->sk_dst_cache == NULL) return NULL;
	if (sk->sk_dst_cache->dev == NULL) return NULL;

	skb_set_owner_w(skb, sk);
	set_bit(SOCKWQ_ASYNC_NOSPACE, &sk->sk_socket->flags);
	set_bit(SOCK_NOSPACE, &sk->sk_socket->flags);

	skb->priority = sk->sk_priority;
	skb_dst_set(skb, dst_clone(sk->sk_dst_cache));
	skb->dev      = sk->sk_dst_cache->dev;
	skb->protocol = __constant_htons(ETH_P_IP);
	
	udppacketlen = RTPBuildRTPHeader(skb) + sizeof(struct udphdr);

	skb_push(skb, sizeof(struct udphdr));
	skb_reset_transport_header(skb);
	udph = udp_hdr(skb);
//	udph->dest = sk->dport;
//	udph->source = sk->sport;
	udph->dest = htons(s->parm.send_port);
	udph->source = htons(s->parm.recv_port);
	udph->len = htons(udppacketlen);
	udph->check = 0;
	
	inet = inet_sk(sk);
	skb_push(skb, sizeof(struct iphdr));
	skb_reset_network_header(skb);
	iph = ip_hdr(skb);
	iph->version = 4;
	iph->ihl = 5;

	iph->frag_off = __constant_htons(IP_DF);
	iph->id = inet->inet_id++;
	iph->ttl = inet->uc_ttl;
//	iph->daddr = sk->daddr;
	iph->daddr = s->parm.send_addr;
	iph->saddr = inet->inet_saddr;
	iph->protocol = IPPROTO_UDP;
	iph->tot_len = htons(sizeof(struct iphdr) + udppacketlen);
	iph->tos = inet->tos;
	
	return skb;
}


void rtp_send_mirror_skb(struct sk_buff *skb_in)
{
	session_t* s = skb_session(skb_in);
	struct sk_buff *skb = dev_alloc_skb(1500);
	struct dst_entry *dst;
	struct neighbour *neigh;
	struct iphdr* iph;
	void *data;
	unsigned long copied;
	
	if (skb == NULL) return;
	skb_reserve(skb, ETH_RTP_HDR_SIZE);

	data=(void *)skb_put(skb, s->pcm_frame_size);
	copied = copy_from_user(data,skb_in->data,s->pcm_frame_size);
	skb_pull(skb_in, s->pcm_frame_size);

	skb_payloadtype(skb)=s->parm.tx_pt;
	skb_session(skb)=s;
	skb_ts(skb)=s->pcm_frame_size;
	
	if(RTPBuildHeaders(skb)) {
		iph = ip_hdr(skb);
		ip_send_check(iph);
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,1,0)
		neigh_connected_output(skb);
#else
		dst = skb_dst(skb_in);
		neigh = dst_neigh_lookup_skb(dst, skb_in);
		neigh_connected_output(neigh, skb);
#endif
	}
	else dev_kfree_skb(skb);

}

void rtp_send_skb(struct sk_buff *skb)
{
	struct dst_entry *dst;
	struct neighbour *neigh;
	struct iphdr* iph;
	if(skb==NULL) return;

	if(RTPBuildHeaders(skb)) {
		iph = ip_hdr(skb);
		
		ip_send_check(iph);
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,1,0)
		neigh_connected_output(skb);
#else
		dst = skb_dst(skb);
		neigh = dst_neigh_lookup_skb(dst, skb);
		neigh_connected_output(neigh, skb);
#endif
	}
	else dev_kfree_skb(skb);
}

#endif
