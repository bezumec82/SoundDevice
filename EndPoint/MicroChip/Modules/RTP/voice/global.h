#ifndef __GLOBAL_H
#define __GLOBAL_H
#define DEBUG 1
//#undef DEBUG
#undef MEMORY_DEBUG
#include <linux/interrupt.h>
#include "sysdep.h"
#include "msec-sched.h"

#include "hal.h"
#include "dsp-hal.h"
#include "rtp.h"
#include "rtp_ext.h"
#include "channel.h"
#include "codec.h"
#include "cb.h"


#define ETH_RTP_HDR_SIZE ((sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + RTP_HEADER_LENGTH + RTP_EXT_PAYLOAD_MAX_SIZE) & ~0x0003)

#ifdef DEBUG
#define RPRINTK(fmt, args...) printk(KERN_DEBUG "%s: " fmt, __FUNCTION__ , ## args)
#define DPRINTK(fmt, args...) printk(KERN_DEBUG "%s: " fmt, __FUNCTION__ , ## args)
#else
#define RPRINTK(fmt, args...)
#define DPRINTK(fmt, args...)
#endif


#endif
