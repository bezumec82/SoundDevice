#include "sysdep.h"
#include "rtp.h"
#include "rtp_api.h"
#include <linux/pkt_sched.h>

#define _SYS_INVALID_SOCKET			-1
#define _SYS_SOCKET_ERROR			-1
#define _SYS_SOCKET_ADDRINUSE   	-2
#define _SYS_SOCKET_ADDRNOTAVAIL	-3

void set_error(const char* fname, int errcode){ printk("Error %d in function: %s\n", errcode, fname);}

void rtp_close_connection(session_t* s)
{
	if (s == NULL) return;
	if (s->socket) sock_release(s->socket);
	s->socket = NULL;
}

static __inline__ struct socket* _sys_create_socket(int type)
{
	struct socket *skt;
	if (sock_create(PF_INET, type, IPPROTO_UDP, &skt) < 0)
		return (struct socket*) _SYS_INVALID_SOCKET;

	else
		return skt;
}

static __inline__ int _sys_set_socket_tos_realtime(struct socket* skt)
{
	//printk("Try SET _sys_set_socket_tos...\n");
	//if ((skt->ops->setsockopt)&&(skt->ops->setsockopt(skt,SOL_IP, IP_TOS, &opt, sizeof(opt)) < 0)){
	//if (sock_setsockopt(skt, SOL_IP, IP_TOS, &opt, sizeof(opt)) < 0){
	//	return _SYS_SOCKET_ERROR;
	//}
	//else

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	//
	lock_sock(skt->sk);
		skt->sk->protinfo.af_inet.tos=IPTOS_LOWDELAY;
		skt->sk->priority = TC_PRIO_MAX;//TC_PRIO_CONTROL;//rt_tos2priority(opt);
		sk_dst_reset(skt->sk);
	release_sock(skt->sk);
	//
#else
	struct inet_sock *inet = inet_sk(skt->sk);
	if (inet)
	{
		lock_sock(skt->sk);
			inet->tos = IPTOS_LOWDELAY;
			skt->sk->sk_priority = TC_PRIO_MAX;
			sk_dst_reset(skt->sk);
		release_sock(skt->sk);
	}
#endif
	

	//skt->sk->protinfo.af_inet.tos=opt; //?
	//struct ip_options *opt = NULL;
    //struct rtable *rt = inet_sk(skt->sk)->cork.rt;

	return 0;
}

static __inline__ int _sys_connect_socket(struct socket* skt, struct sockaddr_in *sa)
{
	if (skt->ops->connect(skt, (struct sockaddr *) sa, sizeof(struct sockaddr_in), 0) < 0)
		return _SYS_SOCKET_ERROR;
	else return 0;
}

static __inline__ int _sys_bind(struct socket* skt, struct sockaddr_in *sa)
{
	int error = skt->ops->bind(skt, (struct sockaddr *) sa, sizeof(*sa));
	if (error < 0) {
		if (error == -EADDRINUSE)
			return (int) _SYS_SOCKET_ADDRINUSE;

		else if (error == -EADDRNOTAVAIL)
			return (int) _SYS_SOCKET_ADDRNOTAVAIL;

		else
			return (int) _SYS_SOCKET_ERROR;
	} else
		return 0;
}
     
/*     
#ifndef WORDS_BIGENDIAN
static __inline__ void FlipRTPByteOrdering(char *the_packet, int pktlen)
{
	rtp_hdr_t *the_hdr = (rtp_hdr_t *) the_packet;
	rtp_packet the_pkt;
	the_hdr->seq = ntohs(the_hdr->seq);
	the_hdr->ts = ntohl(the_hdr->ts);
	the_hdr->ssrc = ntohl(the_hdr->ssrc);
	the_pkt = RTPGetRTPPacket(the_packet, pktlen);

	// SSRC and CSRC stay in network byte order
	if (the_pkt.RTP_extension != NULL) {
		the_pkt.RTP_extension->etype = ntohs(the_pkt.RTP_extension->etype);
		the_pkt.RTP_extension->elen = ntohs(the_pkt.RTP_extension->elen);
	}
}
#else
#define FlipRTPByteOrdering(the_packet, pktlen)
#endif
*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
int rtp_create_connection(session_t* s)
{
	struct sockaddr_in saddr;
	extern void rtp_socket_data_ready(struct sock *sk, int bytes);

	if ((s->parm.send_port == 0) || (s->parm.recv_port == 0))	{
		set_error(__FUNCTION__, RTP_BAD_PORT);
		return -1;
	}

	/* Create the RTP and RTCP sockets for this sender */
	s->socket = _sys_create_socket(SOCK_DGRAM);
	if ((int)s->socket == (int)_SYS_INVALID_SOCKET)	{
		set_error(__FUNCTION__, RTP_CANT_GET_SOCKET);
		return -1;
	}
	
	/* Set realtime options */
	if (_sys_set_socket_tos_realtime(s->socket)) {
		sock_release(s->socket);
		set_error(__FUNCTION__, RTP_CANT_SET_SOCKOPT);
		goto bailout;
	}

//	s->channel = NULL;
	/* init socket RX callback */
	s->socket->sk->user_data = (void*)s;
	s->socket->sk->data_ready = rtp_socket_data_ready;

	/* If the port is odd, assume it's the RTCP port */
	s->parm.send_port &= 0xFFFE;	
	s->parm.recv_port &= 0xFFFE;

	/* Connect RTP socket */
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;

	/* RTP port bind. Receive packets from any sources */
	saddr.sin_port = htons(s->parm.recv_port);
	saddr.sin_addr.s_addr = INADDR_ANY;
	if (_sys_bind(s->socket, &saddr) == (int) _SYS_SOCKET_ADDRNOTAVAIL) {
		set_error(__FUNCTION__, RTP_CANT_BIND_SOCKET);
		goto bailout;
	}

	/* RTP port connect. Init destination cache */
	saddr.sin_port = htons(s->parm.send_port);
	saddr.sin_addr.s_addr = s->parm.send_addr;
	if ((int)_sys_connect_socket(s->socket, &saddr) == (int)_SYS_SOCKET_ERROR) {
		set_error(__FUNCTION__, RTP_CANT_BIND_SOCKET);
		goto bailout;
	}
	
	s->socket->sk->dport = 0;
	/* If remote addr is MULTICAST then unconnect. Can receive packets from any hosts */
	if (MULTICAST(s->socket->sk->daddr)) {
		s->socket->sk->daddr = 0;
	}

    return RTP_OK;

bailout:
	rtp_close_connection(s);
	printk("rtp_create_connection: bailing out, destroying everything around...\n");
	return -1;
}
#else
int rtp_create_connection(session_t* s)
{
	struct sockaddr_in saddr;
	struct inet_sock *inet;
	extern void rtp_socket_data_ready(struct sock *sk, int bytes);

	if ((s->parm.send_port == 0) || (s->parm.recv_port == 0))	{
		set_error(__FUNCTION__, RTP_BAD_PORT);
		return -1;
	}

	/* Create the RTP and RTCP sockets for this sender */
	s->socket = _sys_create_socket(SOCK_DGRAM);
	if ((int)s->socket == (int)_SYS_INVALID_SOCKET)	{
		set_error(__FUNCTION__, RTP_CANT_GET_SOCKET);
		return -1;
	}
	
	/* Set realtime options */
	if (_sys_set_socket_tos_realtime(s->socket)) {
		sock_release(s->socket);
		set_error(__FUNCTION__, RTP_CANT_SET_SOCKOPT);
		goto bailout;
	}

//	s->channel = NULL;
	/* init socket RX callback */
	s->socket->sk->sk_user_data = (void*)s;
	s->socket->sk->sk_data_ready = rtp_socket_data_ready;

	/* If the port is odd, assume it's the RTCP port */
	s->parm.send_port &= 0xFFFE;	
	s->parm.recv_port &= 0xFFFE;

	/* Connect RTP socket */
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;

	/* RTP port bind. Receive packets from any sources */
	saddr.sin_port = htons(s->parm.recv_port);
	saddr.sin_addr.s_addr = INADDR_ANY;
	if (_sys_bind(s->socket, &saddr) == (int) _SYS_SOCKET_ADDRNOTAVAIL) {
		set_error(__FUNCTION__, RTP_CANT_BIND_SOCKET);
		goto bailout;
	}

	/* RTP port connect. Init destination cache */
	saddr.sin_port = htons(s->parm.send_port);
	saddr.sin_addr.s_addr = s->parm.send_addr;
	if ((int)_sys_connect_socket(s->socket, &saddr) == (int)_SYS_SOCKET_ERROR) {
		set_error(__FUNCTION__, RTP_CANT_BIND_SOCKET);
		goto bailout;
	}
	
	inet = inet_sk(s->socket->sk);
	if (inet)
	{	
		inet->inet_dport = 0;
		/* If remote addr is MULTICAST then unconnect. Can receive packets from any hosts */
		if (ipv4_is_multicast(inet->inet_daddr)) {
			inet->inet_daddr = 0;
		}
	}

    return RTP_OK;

bailout:
	rtp_close_connection(s);
	printk("rtp_create_connection: bailing out, destroying everything around...\n");
	return -1;
}
#endif
