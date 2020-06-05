#include "global.h"
#include "linux/poll.h"
#include <linux/file.h>

extern int circular_buffer_get(struct s_circular_buffer	*pcb, void *item, int item_size);

#define VOICE_EXT_SLABNAME			"voice_sessions_etx"
#define RTP_EXT_CLASSNAME 			"kvoice_rtp_ext"

#define GET_SESSION_EXT \
	session_ext_t *s; \
	s = (session_ext_t *)file->private_data;\
	if (s==NULL) return -EBADF;

void module_inc_use_count(void);
void module_dec_use_count(void);

unsigned int s_ext_active=0;

static struct file_operations rtp_ext_fops;
static int rtp_ext_major=0;
struct kmem_cache *session_ext_cache=NULL;
static struct class*  rtp_ext_class  = NULL;
static struct device* rtp_ext_device = NULL;

LIST_HEAD(rtp_ext_sessions);
struct semaphore rtp_ext_sessions_lock;

/*----------------------------------------------------------------------------*/
size_t rtp_ext_pack_rx(session_ext_t *s, char *buf, size_t count)
{
	size_t size_buff, pack_size, curr_size = -1;
	if (s && buf && (count > 0))
	{
		curr_size = 0;
		size_buff = sizeof(s->skt_pack_rx_payload);
		memset(s->skt_pack_rx_payload, 0, sizeof(s->skt_pack_rx_payload));
		pack_size = sizeof(s->skt_pack_rx_addr);
		if ((pack_size + curr_size) <= size_buff)
		{
			memcpy(&s->skt_pack_rx_payload[curr_size], &s->skt_pack_rx_addr, pack_size);
			curr_size += pack_size;

			pack_size = count;
			if ((pack_size + curr_size) <= size_buff)
			{
				memcpy(&s->skt_pack_rx_payload[curr_size], buf, pack_size);
				curr_size += pack_size;
			}
		}
	}

	return curr_size;
}
/*----------------------------------------------------------------------------*/
size_t rtp_ext_unpack_rx(session_ext_t *s, char *buf, size_t count)
{
	size_t size_buff, unpack_size, curr_size = -1, curr_pos = 0;
	if (s && buf && (count > 0))
	{
		curr_size = 0;
		size_buff = sizeof(s->int_unpack_rx_payload);
		unpack_size = sizeof(s->int_unpack_rx_addr);
		if (unpack_size <= count)
		{
			memcpy(&s->int_unpack_rx_addr, &buf[curr_pos], unpack_size);
			curr_pos += unpack_size;
			count -= unpack_size;

			unpack_size = count;
			if (unpack_size <= size_buff)
			{
				memcpy(s->int_unpack_rx_payload, &buf[curr_pos], unpack_size);
				curr_size = unpack_size;
			}
		}
	}

	return curr_size;
}
/*----------------------------------------------------------------------------*/
int rtp_ext_signal_rx(session_ext_t *s)
{
	if (s)
	{
		wake_up_interruptible(&s->pwait);
		s->signal_rx = 1;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static int rtp_ext_create(session_ext_t *s)
{
	if (s)
	{
		circular_buffer_init(&s->tx_buf, DATA_BUFFER_LENGTH, 1);
		circular_buffer_init(&s->rx_buf, DATA_BUFFER_LENGTH, 1);
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static int rtp_ext_close(session_ext_t *s)
{
	if (s)
	{
		circular_buffer_free(&s->tx_buf);
		circular_buffer_free(&s->rx_buf);
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static int rtp_ext_clear(session_ext_t *s)
{
	if (s)
	{
		circular_buffer_clear(&s->tx_buf);
		circular_buffer_clear(&s->rx_buf);
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static int rtp_ext_read_source_addr(struct file *file, struct rtp_ext_source_addr* saddr)
{
	struct rtp_ext_source_addr	unpack_saddr;
	int res = 0;
	size_t result = 0, notcopied;
	
	GET_SESSION_EXT
	
	result = circular_buffer_get(&s->rx_buf, s->int_rx_payload, sizeof(s->int_rx_payload));
	if (result > 0)
	{
		result = rtp_ext_unpack_rx(s, s->int_rx_payload, sizeof(s->int_rx_payload));
		if (result > 0)
		{
			unpack_saddr.ssrc				= s->int_unpack_rx_addr.ssrc;
			unpack_saddr.src_addr			= s->int_unpack_rx_addr.src_addr.s_addr;
			unpack_saddr.src_port			= s->int_unpack_rx_addr.src_port;
			unpack_saddr.etype				= s->int_unpack_rx_addr.etype;
			
			notcopied = copy_to_user(saddr, &unpack_saddr, sizeof(unpack_saddr));
			if ( notcopied == 0 )
			{
				res = sizeof(struct rtp_ext_source_addr);
			}
			else
			{
				res = -EFAULT;
			}
		}
		else
		{
			DPRINTK("rtp_ext_unpack_rx read error\n");
			res = -ENOMEM;
		}
	}
	else
	{
		if (result < 0)
		{
			DPRINTK("circular_buffer_get read error\n");
			res = -EFAULT;
		}
	}

	return res;
}
/*----------------------------------------------------------------------------*/
static int rtp_ext_attach_session(struct file *file, struct rtp_ext_connection_params* ios)
{
	struct file *file_rtp_session = NULL;
	session_t *rtp_session = NULL;
	unsigned long copied;
	
	GET_SESSION_EXT
	
	copied = copy_from_user(&s->parm, ios, sizeof(struct rtp_ext_connection_params));

	/* get rtp session */
	file_rtp_session = fget(s->parm.fd_rtp);
	if (file_rtp_session)
	{
		rtp_session = (session_t *)file_rtp_session->private_data;
		fput(file_rtp_session);
	}
	/* attach */
	if (rtp_session == NULL) return -ENOENT;
	
	s->rtp_session = rtp_session;
	rtp_session->rtp_ext_session = s;
	rtp_ext_clear(s);
	s->rtp_session->active_rtp_ext = 1;
	s->active = 1;	
	
	return 0;
};
/*----------------------------------------------------------------------------*/
static int rtp_ext_detach_session(struct file *file, unsigned long arg)
{
	GET_SESSION_EXT
	
	s->active = 0;	
	if (s->rtp_session)
	{
		if (s->rtp_session->rtp_ext_session)
		{
			s->rtp_session->active_rtp_ext = 0;
			s->rtp_session->rtp_ext_session	= NULL;			
		}
		s->rtp_session = NULL;
		rtp_ext_clear(s);
	}
	return 0;
};
/*----------------------------------------------------------------------------*/
static int rtp_ext_open(struct inode *inode, struct file *file)
{
	session_ext_t* s;
	
	module_inc_use_count();	
	s = kmem_cache_alloc(session_ext_cache, GFP_KERNEL);
	if (s==NULL) return -ENOMEM;

	memset(s, 0, sizeof(session_ext_t));
	rtp_ext_create(s);
	init_waitqueue_head(&s->pwait);
	file->private_data = s;
	INIT_LIST_HEAD(&s->node);
	down(&rtp_ext_sessions_lock);
	list_add(&s->node, &rtp_ext_sessions);
	up(&rtp_ext_sessions_lock);
	s_ext_active++;	
	return 0;
}
/*----------------------------------------------------------------------------*/
static int rtp_ext_release(struct inode *inode, struct file *file)
{
	module_dec_use_count();
	GET_SESSION_EXT

	/* release network resources */
	local_bh_disable();

	/* close rtp_ext */
	rtp_ext_close(s);
	list_del(&s->node);
	s_ext_active--;

	local_bh_enable();

	kmem_cache_free(session_ext_cache, s);
	return 0;
}
/*----------------------------------------------------------------------------*/
static ssize_t rtp_ext_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	size_t res = 0, notcopied;
	
	GET_SESSION_EXT
	
	res = circular_buffer_get(&s->rx_buf, s->int_rx_payload, sizeof(s->int_rx_payload));
	if (res > 0)
	{
		res = rtp_ext_unpack_rx(s, s->int_rx_payload, res);
		if ((res > 0) && (res <= count))
		{
			notcopied = copy_to_user(buf, s->int_unpack_rx_payload, res);			
			
			if ( notcopied == 0 )
			{
				circular_buffer_pop(&s->rx_buf, s->int_rx_payload, sizeof(s->int_rx_payload));
			}
			else
			{
				res = -EFAULT;
			}
		}
		else
		{
			res = -ENOMEM;
		}
	}
	else
	{
		if (res < 0)
		{
			DPRINTK("circular_buffer_get read error\n");
			res = -EFAULT;
		}
	}
	
	return res;
}
/*----------------------------------------------------------------------------*/
static ssize_t rtp_ext_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	size_t res = 0, notcopied;
	
	GET_SESSION_EXT
	
	if (count <= sizeof(s->int_tx_payload))
	{
		notcopied = copy_from_user(s->int_tx_payload, buf, count);
		if ( notcopied == 0 )
		{
			res = circular_buffer_push(&s->tx_buf, s->int_tx_payload, count);
			if (res < 0)
			{
				DPRINTK("RTP_EXT:circular_buffer_push tx_buf error\n");
				res = -EFAULT;
			}
		}
		else
		{
			res = -EFAULT;
		}
	}
	else
	{
		res = -ENOMEM;
	}
	
	return res;
}
/*----------------------------------------------------------------------------*/
unsigned int rtp_ext_poll(struct file *file, struct poll_table_struct *pt)
{
	GET_SESSION_EXT

	poll_wait(file, &s->pwait, pt);
	if (s->signal_rx || circular_buffer_lenght(&s->rx_buf) > 0)
	{
		s->signal_rx = 0;
		return POLLIN;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
int rtp_ext_ctl_init(void *context)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0) 
	session_ext_cache = kmem_cache_create( VOICE_EXT_SLABNAME, sizeof(session_ext_t), 0, SLAB_HWCACHE_ALIGN, NULL, NULL);
#else 
	session_ext_cache = kmem_cache_create( VOICE_EXT_SLABNAME, sizeof(session_ext_t), 0, SLAB_HWCACHE_ALIGN, NULL);
#endif 
	if(!session_ext_cache) return -ENOMEM;
	rtp_ext_major = register_chrdev(0, RTP_EXT_DEVNAME, &rtp_ext_fops);
	if (rtp_ext_major < 0)
	{
		kmem_cache_destroy(session_ext_cache);
		printk(KERN_DEBUG  "failed to register device \"" RTP_EXT_DEVNAME "\"\n");
		return -EBUSY;
	}
	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	rtp_ext_class = class_create(THIS_MODULE, RTP_EXT_CLASSNAME);
	if (IS_ERR(rtp_ext_class))
	{
		kmem_cache_destroy(session_ext_cache);
		unregister_chrdev(rtp_ext_major, RTP_EXT_DEVNAME);
		printk(KERN_ALERT "Failed to register device class '%s'\n", RTP_EXT_CLASSNAME);
		return PTR_ERR(rtp_ext_class);
	}

	rtp_ext_device = device_create(rtp_ext_class, NULL, MKDEV(rtp_ext_major, 0), NULL, RTP_EXT_DEVNAME);
	if (IS_ERR(rtp_ext_device))
	{
		class_destroy(rtp_ext_class);
		kmem_cache_destroy(session_ext_cache);
		unregister_chrdev(rtp_ext_major, RTP_EXT_DEVNAME);
		printk(KERN_ALERT "Failed to create the device '%s'\n", RTP_EXT_DEVNAME);
		
		return PTR_ERR(rtp_ext_device);
	}
#endif
	
	INIT_LIST_HEAD(&rtp_ext_sessions);
	sema_init(&rtp_ext_sessions_lock, 1);
	return 0;
}
/*----------------------------------------------------------------------------*/
void rtp_ext_ctl_deinit(void *context)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	device_destroy(rtp_ext_class, MKDEV(rtp_ext_major, 0));
	class_unregister(rtp_ext_class);
	class_destroy(rtp_ext_class);
#endif
	unregister_chrdev(rtp_ext_major, RTP_EXT_DEVNAME);
	if(session_ext_cache) kmem_cache_destroy(session_ext_cache);
}
/*----------------------------------------------------------------------------*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
int rtp_ext_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#else
long rtp_ext_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
	switch (cmd)
	{
		case RTP_EXT_ATTACH_SESSION: 	return rtp_ext_attach_session(file, (struct rtp_ext_connection_params*) arg);
		case RTP_EXT_DETACH_SESSION: 	return rtp_ext_detach_session(file, arg);
		case RTP_EXT_READ_SOURCE_ADDR: 	return rtp_ext_read_source_addr(file, (struct rtp_ext_source_addr*) arg);
	}
	return -EINVAL;
}
/*----------------------------------------------------------------------------*/
static struct file_operations rtp_ext_fops = {
	open: rtp_ext_open,
	read: rtp_ext_read,
	write: rtp_ext_write,
	release: rtp_ext_release,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	ioctl: rtp_ext_ioctl,
#else
    unlocked_ioctl:rtp_ext_ioctl,
    compat_ioctl:rtp_ext_ioctl,
#endif
	poll: rtp_ext_poll
};
/*----------------------------------------------------------------------------*/

