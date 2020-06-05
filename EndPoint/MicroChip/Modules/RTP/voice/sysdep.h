/* System-dependent header files */

#ifndef _SYSDEP_H
#define _SYSDEP_H

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/time.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/utsname.h>
#include <linux/sched.h>
#include <linux/delay.h>


#include <asm/errno.h>
#include <stdarg.h>
#include <asm/uaccess.h>
#include <asm/byteorder.h>

#include <net/sock.h>
#include <net/udp.h>
#include <net/ip.h>

/* #include <linux/devfs_fs_kernel.h> */
#include <linux/fs.h>
#include <linux/proc_fs.h>

#endif /* _SYSDEP_H */
