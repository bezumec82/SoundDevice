#ifndef __WORKSPACE_H_
#define __WORKSPACE_H_


#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/poll.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <linux/sound.h>
#include <linux/soundcard.h>
#include <linux/major.h>
#include <linux/signal.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/ioctl.h>
#include <linux/proc_fs.h>
#include <linux/rtnetlink.h>

#include <asm/io.h>
#include <asm/segment.h>
#include <asm/poll.h>
#include <asm/atomic.h>

#undef DEBUG
#ifdef DEBUG

#define DPRINTK(fmt, args...) printk(KERN_DEBUG "%s: " fmt, __FUNCTION__ , ## args)
#else
#define DPRINTK(fmt, args...)
#endif


#define MAX_ENABLE_STRS	64

#endif /* __WORKSPACE_H_ */


