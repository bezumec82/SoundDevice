#include "global.h"
#include "sysdep.h"
#include "rtp.h"
#include "rtp_ext.h"
#include "rtp_api.h"

static inline rtp_packet RTPGetRTPPacket(char *rtppacket, long pktlen)
{
	rtp_packet the_packet;

	/* If packet is encrypted, then the extra 32-bit header used for
	 * encryption should have already been stripped off	 */
	the_packet.RTP_header = (rtp_hdr_t *) rtppacket;
	if (RTP_X(*the_packet.RTP_header) == 1)
	{   /* Is there an extension? */
		/* if so, then set up RTP_extension field.  */
		the_packet.RTP_extension = (rtp_hdr_ext *) ((char *) rtppacket + sizeof(rtp_hdr_t) + (RTP_CC(*the_packet.RTP_header) - 1) * 4);
		/* Note that if the packet has not yet been converted to host-byte-
		 * ordering, then the extension length will be wrong, and so the
		 * packet payload will point to the wrong place. */
		the_packet.payload = ((char *) the_packet.RTP_extension + (the_packet.RTP_extension->elen + 1) * 4);
	}
	/* Otherwise, set extension field to NULL */
	else {
		the_packet.RTP_extension = NULL;
		the_packet.payload = ((char *) rtppacket + sizeof(rtp_hdr_t) + (RTP_CC(*the_packet.RTP_header) - 1) * 4);
	}
	the_packet.payload_len = pktlen + (long) (rtppacket - the_packet.payload);
	if (RTP_P(*the_packet.RTP_header) == 1) {
		/* There is padding on this packet.  The # of bytes of padding (stored
		 * in the last byte of the packet = where we currently point to as the
		 * last byte in the payload) must be subtracted from our current
		 * payload estimate.
		 */
		the_packet.payload_len -= (int) the_packet.payload[the_packet.payload_len - 1];
	}
	return the_packet;
}

extern u32 ts_timer(void);

void rtp_socket_data_ready(struct sock *sk, int bytes)
{
	struct sk_buff *skb;
	rtp_packet packet;
	int error;
	while (1)
	{
		skb = skb_recv_datagram(sk, MSG_DONTWAIT, 1, &error);
		if (!skb) return;

		skb_pull(skb, sizeof(struct udphdr));

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		sk->stamp = skb->stamp;
#else
		sk->sk_stamp = skb->tstamp;
#endif

		packet = RTPGetRTPPacket((char *)skb->data, skb->len);
		skb_pull(skb, (unsigned long) packet.payload - (unsigned long) skb->data);
		
		skb_payloadtype(skb)=packet.RTP_header->mpt & 0x7F;
		skb_seq(skb)=packet.RTP_header->seq;
		skb_ts(skb)=packet.RTP_header->ts;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)		
		skb_session(skb)=sk->user_data;
#else
		skb_session(skb)=sk->sk_user_data;
#endif
		skb_marker(skb)=packet.RTP_header->mpt & 0x80 ? 1: 0;
		skb_localts(skb)=ts_timer();
		skb_ssrc(skb)=packet.RTP_header->ssrc;
		skb_payload_len(skb)=packet.payload_len;

		rtp_ext_rx_callback(skb, &packet);
		
		if (rtp_rx_filter_callback(skb, &packet))
		{
			rtp_rx_callback(skb);
			/* DEBUG - DO MIRROR
			rtp_send_mirror_skb(skb);
			*/
		}
		else
		{
			session_t* s = skb_session(skb);
			s->stat.rx_dropped++;
			dev_kfree_skb(skb);
		}
	}
	
	//if (skb->len) skb_pull(skb,skb->len);
	
//	dev_kfree_skb(skb);
}
