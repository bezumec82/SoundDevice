/* vim: set ts=4 : */

/*
 * ft245.c  Version 2.0 (kernel 2.6)
 *
 * Copyright (c) 2004,2006 Robin Cutshaw	<robin@hamlabs.com>
 *
 * USB driver for FTDI FT-245 based devices
 * including the RFSpace Software Defined Radio - SDR-14
 *
 * This driver is optimized for high speed data transfer from the
 * RFSpace SDR-14 and can be used for other FTDI FT245 based devices.
 *
 * ChangeLog:
 *	v1.0  - initial release (kernel 2.4)
 *	v2.0  - rewrite for kernel 2.6
 *	v2.0.1  - minor fix to show device name
 *	v2.0.2  - add SDR-IQ device
 *	v2.0.3  - modified for kernel 2.6.19+
 *	v2.0.4  - modified for kernel 2.6.35+
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


/* Theory of operation
 * 
 * usb_ft245_init() is called when the driver is loaded
 * ft245_probe() is called when a device matching the vendor/product is inserted
 * ft245_open() is called when a user process opens the device for access
 *            the device may only be opened by one process at a time
 *            upon open, a read callback is issued to queue a device read
 * ft245_read_bulk_callback() is called when data is delivered from the device
 *                          data is delivered in the callbackbuf[]
 *                          then data is placed into rbufp[] when
 *                             a complete packet has been processed
 * ft245_read() is called when a user process reads from the device
 *            data is read from the rbufp[]
 * ft245_write() is called when a user process writes to the device
 * ft245_poll() is called when a user process calls select() or poll()
 * ft245_release() is called when a user process closes the device
 * ft245_disconnect() is called when a device is unplugged from the usb bus
 * usb_ft245_cleanup() is called when the driver is unloaded
 *                     it is possible that cleanup is called before
 *                     disconnect()
 * 
 * The FTDI FT245 usb device sends packets of 64 or less bytes at a time
 * with the first two bytes always being modem status bytes.  We ignore
 * these bytes since the 245 has a parallel interface.
 */

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#include <linux/sched/signal.h>
#else
#include <linux/sched.h>
#endif
//#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18)
//#include <linux/config.h>
//#else
//#include <linux/autoconf.h>
//#endif
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/kref.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/usb.h>

#if HAVE_UNLOCKED_IOCTL
#include <linux/mutex.h>
#else
#include <linux/smp_lock.h>
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
#define usb_alloc_coherent usb_buffer_alloc
#define usb_free_coherent usb_buffer_free
#endif

#define FT245_DBG_ERROR		0x001
#define FT245_DBG_INIT		0x002
#define FT245_DBG_PROBE		0x004
#define FT245_DBG_OPENCLOSE	0x008
#define FT245_DBG_POLL		0x010
#define FT245_DBG_WRITE		0x020
#define FT245_DBG_CONTROL	0x040
#define FT245_DBG_READ		0x080
#define FT245_DBG_READCALL	0x100
#define FT245_DBG_ALL		0x1ff

static int debug = 0x007;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29)
#define ft245dbg(level, format, arg...) do { if (debug&level) info(format, ## arg); } while(0)
#define ft245info(format, arg...) do { info(format, ## arg); } while(0)
#define ft245warn(format, arg...) do { warn(format, ## arg); } while(0)
#else
#define ft245dbg(level, format, arg...) do { if (debug&level) pr_info(format, ## arg); } while(0)
#define ft245info(format, arg...) do { pr_info(format, ## arg); } while(0)
#define ft245warn(format, arg...) do { pr_warn(format, ## arg); } while(0)
#endif


#define DRIVER_VERSION "v2.0.3"
#define DRIVER_AUTHOR "Robin Cutshaw, <robin@hamlabs.com>"
#define DRIVER_DESC "USB Driver for FTDI FT245 chipset"

module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Debug level");

#define	USB_FT245_MINOR_BASE	144

#define	READ_BUFFER_SIZE	0x10000		// 64k
#define	MAX_REQ_PACKET_SIZE	0x10000		// 64k
#define	RETRY_TIMEOUT		(HZ)
#define	MAX_WRITE_RETRY		5

#define FT245_FLAGS_DEV_OPEN		0x01
#define FT245_FLAGS_RX_BUSY		0x02
#define FT245_FLAGS_INTR_BUSY		0x04
#define FT245_FLAGS_DEV_ERROR		0x08
#define FT245_FLAGS_PENDING_CLEANUP	0x10
#define FT245_FLAGS_MASK		0x1f

/*#define USB_FTDI_VID		0x0403*/
#define USB_FTDI_VID		0xfff0    /*LIRA*/
//#define USB_SDR14_DID		0xf728    /*SRC-1*/
//#define USB_SDRIQ_DID		0x6001    /*SRC-2*/
#define USB_SDR03_DID		0x8003    //tangenta
#define USB_SDR04_DID		0x8004    //speaker
#define USB_SDR08_DID		0x8008    //microphone
#define USB_SDR09_DID		0x8009    //headset
#define USB_SDR0D_DID		0x800d    //iline
#define USB_SDR0E_DID		0x800e    //tmicrophone
#define USB_SDR0F_DID		0x800f    //handset
#define USB_SDR20_DID		0x8020    //control

static struct usb_device_id ft245_table [] = {
	/* These have the same application level protocol */  
	//	{ USB_DEVICE(USB_FTDI_VID, USB_SDR14_DID) },	// SDR-14
	//	{ USB_DEVICE(USB_FTDI_VID, USB_SDRIQ_DID) },	// SDR-IQ
	{ USB_DEVICE(USB_FTDI_VID, USB_SDR03_DID) },	// 
	{ USB_DEVICE(USB_FTDI_VID, USB_SDR04_DID) },	// 
	{ USB_DEVICE(USB_FTDI_VID, USB_SDR08_DID) },	// 
	{ USB_DEVICE(USB_FTDI_VID, USB_SDR09_DID) },	// 
	{ USB_DEVICE(USB_FTDI_VID, USB_SDR0D_DID) },	// 
	{ USB_DEVICE(USB_FTDI_VID, USB_SDR0E_DID) },	// 
	{ USB_DEVICE(USB_FTDI_VID, USB_SDR0F_DID) },	// 
	{ USB_DEVICE(USB_FTDI_VID, USB_SDR20_DID) },	// 
	{}					/* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, ft245_table);


struct usb_ft245 {
	struct usb_device				*udev;		/* USB device handle */
	struct usb_interface			*interface;
	__u8							inEP;		/* read endpoint */
	__u8							outEP;		/* write endpoint */
	struct kref						kref;

	char							*rbufp;		/* read buffer for I/O */
	int								rbufhead;
	int								rbufcnt;

	char							*callbackbuf; /* read buffer for callback */
	int								callbackbufsize;

	char							*wbufp;		/* write buffer for I/O */
	int								wbufsize;

	struct urb						*rx_urb;
	int								flags;
	int								roverruns;
	int								roverruncnt;
	int								usboverruns;
	int								firstopen;  /* fix first write problem */

	struct semaphore				sem;

	/* always valid */
	wait_queue_head_t				wait;		/* for timed waits */
#if HAVE_UNLOCKED_IOCTL
	struct mutex fs_mutex; /* for lock while terminate */
#endif
};


static int		ft245_release(struct inode *, struct file *);
static void		ft245_disconnect(struct usb_interface *);
static int		ft245_probe(struct usb_interface *,
	   						const struct usb_device_id *);
static ssize_t	ft245_read(struct file *, char *, size_t, loff_t *);
static ssize_t	ft245_write(struct file *, const char *, size_t, loff_t *);
static ssize_t  ft245_kern_write(struct usb_ft245 *, const char *, size_t);
static int		ft245_open(struct inode *, struct file *);
static unsigned int ft245_poll(struct file *, struct poll_table_struct *);
static void		ft245_dump_buf(unsigned char *, unsigned char *, int);
static void		ft245_cleanup(struct usb_ft245 *);
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18)
static void 	ft245_read_bulk_callback(struct urb *, struct pt_regs *);
#else
static void 	ft245_read_bulk_callback(struct urb *);
#endif
static void		ft245_delete(struct kref *);


static struct file_operations ft245_fops = {
	.owner =	THIS_MODULE,
	.read =		ft245_read,
	.write =	ft245_write,
	.open =		ft245_open,
	.release =	ft245_release,
	.poll =		ft245_poll,
};


static struct usb_driver ft245_driver = {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
	.owner =		THIS_MODULE,
#endif
	.name =			"ft245",
	.probe =		ft245_probe,
	.disconnect =	ft245_disconnect,
	.id_table =		ft245_table,
};


static struct usb_class_driver ft245_class = {
	.name =			"usb/ft245%d",
	.fops =			&ft245_fops,
	.minor_base =	USB_FT245_MINOR_BASE
};


static int ft245_probe(struct usb_interface *interface,
	   					const struct usb_device_id *ft245_info)
{
	struct usb_ft245	*ft245dev = NULL;
	struct usb_host_interface	*iface_desc = NULL;
	struct usb_endpoint_descriptor	*endpoint;
	int				i, retval = -ENOMEM;

	if ((ft245dev = kmalloc(sizeof(struct usb_ft245), GFP_KERNEL)) == NULL) {
		return -ENODEV;
	}
	memset(ft245dev, 0, sizeof(struct usb_ft245));
	kref_init(&ft245dev->kref);

	ft245dev->udev = usb_get_dev(interface_to_usbdev(interface));
	ft245dev->interface = interface;
	
	ft245dbg(FT245_DBG_PROBE, "ft245_probe mfg/product/sn %s/%s/%s",
		ft245dev->udev->manufacturer,
		ft245dev->udev->product,
		ft245dev->udev->serial == NULL ? "none" : ft245dev->udev->serial);


	init_waitqueue_head (&ft245dev->wait);
	sema_init(&ft245dev->sem, 1);
	mutex_init(&ft245dev->fs_mutex);

	iface_desc = interface->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;

		if (!ft245dev->inEP &&
			(endpoint->bEndpointAddress & USB_DIR_IN) &&
			((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			/* we found a bulk in endpoint */
			ft245dev->callbackbufsize = le16_to_cpu(endpoint->wMaxPacketSize);
			ft245dbg(FT245_DBG_PROBE, "max packet size read %d",
				ft245dev->callbackbufsize);
			ft245dev->callbackbufsize = 4096; // override max size
			ft245dev->inEP = endpoint->bEndpointAddress;
			ft245dev->callbackbuf = kmalloc(ft245dev->callbackbufsize, GFP_KERNEL);
			if (!ft245dev->callbackbuf) {
				dev_err(&ft245dev->udev->dev,"Could not allocate callbackbuf");
				goto error;
			}
		}

		if (!ft245dev->outEP &&
			!(endpoint->bEndpointAddress & USB_DIR_IN) &&
			((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			/* we found a bulk out endpoint */
			ft245dev->wbufsize = le16_to_cpu(endpoint->wMaxPacketSize);
			ft245dbg(FT245_DBG_PROBE, "max packet size write %d",
				ft245dev->wbufsize);
			ft245dev->outEP = endpoint->bEndpointAddress;
			ft245dev->wbufp = kmalloc (ft245dev->wbufsize, GFP_KERNEL);
			if (!ft245dev->wbufp) {
				dev_err(&ft245dev->udev->dev,"Could not allocate wbufp");
				goto error;
			}
		}
	}
	if (!(ft245dev->inEP && ft245dev->outEP)) {
		dev_err(&ft245dev->udev->dev,"Could not find both bulk-in and bulk-out endpoints");
		goto error;
	}

	if (!(ft245dev->rbufp = (char *) kmalloc (READ_BUFFER_SIZE, GFP_KERNEL))) {
		dev_err(&ft245dev->udev->dev,"no memory!");
		goto error;
	}
	ft245dev->rbufhead = 0;
	ft245dev->rbufcnt = 0;
	ft245dev->firstopen = 1;
	if ((ft245dev->rx_urb = usb_alloc_urb(0, GFP_KERNEL)) == (struct urb *)NULL) {
		ft245dbg(FT245_DBG_ERROR, "usb_alloc_urb failed");
		goto error;
	}


	/* save our data pointer in this interface device */
	usb_set_intfdata(interface, ft245dev);

	/* we can register the device now, as it is ready */
	retval = usb_register_dev(interface, &ft245_class);
	if (retval) {
		/* something prevented us from registering this driver */
		dev_err(&ft245dev->udev->dev,"Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	ft245info("USB FT245 #%d (/dev/ft245%d) connected, major/minor %d/%d",
		interface->minor - USB_FT245_MINOR_BASE,
		interface->minor - USB_FT245_MINOR_BASE,
		USB_MAJOR, interface->minor);

	return 0;

error:
	if (ft245dev)
		kref_put(&ft245dev->kref, ft245_delete);
	return retval;
}


static int ft245_open(struct inode *inode, struct file *file)
{
	struct usb_ft245	*ft245dev;
	struct usb_interface	*interface;
	char buf[4];
	int			subminor;
	int			retval = 0;
	int			res;

	ft245dbg(FT245_DBG_OPENCLOSE, "ft245_open");
	subminor = iminor(inode);
	interface = usb_find_interface(&ft245_driver, subminor);
	if (!interface) {
		return -ENODEV;
	}

	ft245dev = usb_get_intfdata(interface);
	if (!ft245dev) {
		return -ENODEV;
	}
	down (&(ft245dev->sem));

	if (ft245dev->flags & FT245_FLAGS_DEV_OPEN) {
		retval = -EBUSY;
		goto done;
	}

	kref_get(&ft245dev->kref);
	file->private_data = ft245dev;

	ft245dbg(FT245_DBG_OPENCLOSE, "open #%d", subminor); 

	ft245dev->rbufhead = 0;
	ft245dev->rbufcnt = 0;
	ft245dev->flags = FT245_FLAGS_DEV_OPEN;
	ft245dev->roverruns = 0;
	ft245dev->roverruncnt = 0;
	ft245dev->usboverruns = 0;

	usb_fill_bulk_urb(ft245dev->rx_urb, ft245dev->udev,
		usb_rcvbulkpipe(ft245dev->udev, ft245dev->inEP),
		ft245dev->callbackbuf, ft245dev->callbackbufsize,
		ft245_read_bulk_callback, ft245dev);
	if ((res = usb_submit_urb(ft245dev->rx_urb, GFP_KERNEL)))
		ft245warn("ft245 failed submit rx_urb %d", res);

	up (&(ft245dev->sem));
	if (ft245dev->firstopen) {
		buf[0] = 0x03;
		buf[1] = 0x60;
		buf[2] = 0x03;
		ft245_kern_write(ft245dev, buf, 3);
		ft245dev->firstopen = 0;
	}

	return retval;

done:
	up (&(ft245dev->sem));
	return retval;
}


static ssize_t ft245_read(struct file *file, char *buf, size_t len, loff_t *ppos)
{
	struct usb_ft245	*ft245dev;
	int			retval = 0;

	if (len > MAX_REQ_PACKET_SIZE) {
		ft245dbg(FT245_DBG_ERROR, "ft245_read %d too large", (int)len);
		return -EINVAL;
	}

	ft245dev = (struct usb_ft245 *) file->private_data;
	down (&ft245dev->sem);
	if (!ft245dev->udev) {
		ft245dbg(FT245_DBG_ERROR, "ft245_read %d no dev!", (int)len);
		retval = -ENODEV;
		goto done;
	}
	if (ft245dev->flags & FT245_FLAGS_DEV_ERROR) {
		ft245dbg(FT245_DBG_ERROR, "ft245_read device error");
		retval = -EIO;
		goto done;
	}

	if (!(ft245dev->flags & FT245_FLAGS_DEV_OPEN)) {
		ft245dbg(FT245_DBG_ERROR, "ft245_read not open! (flags 0x%x) 1",
			ft245dev->flags);
		retval = -ENODEV;
		goto done;
	}

	ft245dbg(FT245_DBG_READ, "ft245_read ask %d avail %d head %d flags 0x%x",
		(int)len, ft245dev->rbufcnt, ft245dev->rbufhead, ft245dev->flags);

	if (/*(ft245dev->flags & FT245_FLAGS_RX_BUSY) || */(ft245dev->rbufcnt == 0)) {
		if (file->f_flags & O_NONBLOCK) {
			retval = -EAGAIN;
			goto done;
		}

		while (/*(ft245dev->flags & FT245_FLAGS_RX_BUSY) ||*/
			   (ft245dev->rbufcnt == 0)) {

			if (signal_pending(current)) {
				retval = -EINTR;
				goto done;
			}
			up (&ft245dev->sem);
			wait_event_interruptible(ft245dev->wait, ft245dev->rbufcnt > 0);
			down(&ft245dev->sem);
			if (!(ft245dev->flags & FT245_FLAGS_DEV_OPEN))
				break;
		}
	}

	if (!(ft245dev->flags & FT245_FLAGS_DEV_OPEN)) {
		ft245dbg(FT245_DBG_ERROR, "ft245_read not open! (flags 0x%x) 2",
			ft245dev->flags);
		retval = -ENODEV;
		goto done;
	}

	if (!ft245dev->udev) {
		ft245dbg(FT245_DBG_ERROR, "ft245_read no dev!");
		retval = -ENODEV;
		goto done;
	}

	if (ft245dev->rbufcnt < len)
		len = ft245dev->rbufcnt;
	if (copy_to_user (buf, &ft245dev->rbufp[ft245dev->rbufhead], len)) {
		retval = -EFAULT;
		goto done;
	} else {
		ft245dbg(FT245_DBG_READ, "ft245_read returned %d head %d count %d", (int)len, ft245dev->rbufhead, ft245dev->rbufcnt);
		retval = len;
		ft245dev->rbufcnt -= len;
		if (ft245dev->rbufcnt == 0)
			ft245dev->rbufhead = 0;
		else
			ft245dev->rbufhead += len;
	}

done:
	up (&ft245dev->sem);
	return retval;
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18)
static void ft245_write_bulk_callback(struct urb *urb, struct pt_regs *regs)
#else
static void ft245_write_bulk_callback(struct urb *urb)
#endif
{
	struct usb_ft245 *ft245dev;

	ft245dev = (struct usb_ft245 *)urb->context;

	/* sync/async unlink faults aren't errors */
	if (urb->status && 
		!(urb->status == -ENOENT || 
		  urb->status == -ECONNRESET ||
		  urb->status == -ESHUTDOWN)) {
		dev_err(&ft245dev->udev->dev,"ft245_write_bulk_callback status %d", urb->status);
		//dbg("%s - nonzero write bulk status received: %d",
		//    __FUNCTION__, urb->status);
	}
	if (urb->status != 0)
		ft245dbg(FT245_DBG_WRITE, "ft245_write_bulk_callback status %d", urb->status);

	/* free up our allocated buffer */
	usb_free_coherent(urb->dev, urb->transfer_buffer_length, 
			urb->transfer_buffer, urb->transfer_dma);
}


static ssize_t ft245_write(struct file *file, const char *user_buffer, size_t count, loff_t *ppos)
{
	struct usb_ft245 *ft245dev;
	int retval = 0;
	struct urb *urb = NULL;
	char *buf = NULL;

	ft245dev = (struct usb_ft245 *)file->private_data;

	/* verify that we actually have some data to write */
	if (count == 0)
		goto exit;

	/* create a urb, and a buffer for it, and copy the data to the urb */
	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) {
		retval = -ENOMEM;
		goto error;
	}

	buf = usb_alloc_coherent(ft245dev->udev, count, GFP_KERNEL, &urb->transfer_dma);
	if (!buf) {
		retval = -ENOMEM;
		goto error;
	}

	if (copy_from_user(buf, user_buffer, count)) {
		retval = -EFAULT;
		goto error;
	}

	/* initialize the urb properly */
	usb_fill_bulk_urb(urb, ft245dev->udev,
			  usb_sndbulkpipe(ft245dev->udev, ft245dev->outEP),
			  buf, count, ft245_write_bulk_callback, ft245dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	ft245dbg(FT245_DBG_WRITE, "ft245_write len %d flags 0x%x urbtbl %d", (int)count, ft245dev->flags, urb->transfer_buffer_length);

	ft245_dump_buf("write ", buf, count < 16 ? count : 16);

	/* send the data out the bulk port */
	retval = usb_submit_urb(urb, GFP_KERNEL);
	if (retval) {
		dev_err(&ft245dev->udev->dev,"%s - failed submitting write urb, error %d", __FUNCTION__, retval);
		goto error;
	}

	/* release our reference to this urb, the USB core will eventually free it entirely */
	usb_free_urb(urb);

exit:
	return count;

error:
	ft245dbg(FT245_DBG_WRITE, "ft245_write error %d len %d flags 0x%x urbtbl %d", retval, (int)count, ft245dev->flags, urb->transfer_buffer_length);
	usb_free_coherent(ft245dev->udev, count, buf, urb->transfer_dma);
	usb_free_urb(urb);
	return retval;
}


static ssize_t ft245_kern_write(struct usb_ft245 *ft245dev, const char *buffer, size_t count)
{
	int retval = 0;
	struct urb *urb = NULL;
	char *buf = NULL;

	/* verify that we actually have some data to write */
	if (count == 0)
		goto exit;

	/* create a urb, and a buffer for it, and copy the data to the urb */
	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) {
		retval = -ENOMEM;
		goto error;
	}

	buf = usb_alloc_coherent(ft245dev->udev, count, GFP_KERNEL, &urb->transfer_dma);
	if (!buf) {
		retval = -ENOMEM;
		goto error;
	}

	memcpy(buf, buffer, count);

	/* initialize the urb properly */
	usb_fill_bulk_urb(urb, ft245dev->udev,
			  usb_sndbulkpipe(ft245dev->udev, ft245dev->outEP),
			  buf, count, ft245_write_bulk_callback, ft245dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	ft245dbg(FT245_DBG_WRITE, "ft245_kern_write len %d flags 0x%x urbtbl %d", (int)count, ft245dev->flags, urb->transfer_buffer_length);

	ft245_dump_buf("write ", buf, count < 16 ? count : 16);

	/* send the data out the bulk port */
	retval = usb_submit_urb(urb, GFP_KERNEL);
	if (retval) {
		dev_err(&ft245dev->udev->dev,"%s - failed submitting write urb, error %d", __FUNCTION__, retval);
		goto error;
	}

	/* release our reference to this urb, the USB core will eventually free it entirely */
	usb_free_urb(urb);

exit:
	return count;

error:
	ft245dbg(FT245_DBG_WRITE, "ft245_kern_write error %d len %d flags 0x%x urbtbl %d", retval, (int)count, ft245dev->flags, urb->transfer_buffer_length);
	usb_free_coherent(ft245dev->udev, count, buf, urb->transfer_dma);
	usb_free_urb(urb);
	return retval;
}


#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18)
static void ft245_read_bulk_callback(struct urb *urb, struct pt_regs *regs)
#else
static void ft245_read_bulk_callback(struct urb *urb)
#endif
{
	struct usb_ft245 *ft245dev = urb->context;
	int  res, rbuftail, callbackndx, chunksize;
	int count = urb->actual_length;

	if (!ft245dev) {
		ft245dbg(FT245_DBG_ERROR, "ft245_read_callback ft245 NULL!");
		return;
	}
	if (urb->status != 0)
		ft245dbg(FT245_DBG_READCALL, "ft245_read_bulk_callback count %d flags %d status %d over %d overcnt %d usbover %d",
			count, ft245dev->flags, urb->status,
			ft245dev->roverruns, ft245dev->roverruncnt, ft245dev->usboverruns);

	if (!(ft245dev->flags & FT245_FLAGS_DEV_OPEN)) {
		ft245dbg(FT245_DBG_READCALL, "ft245_read_bulk_callback dev not open!");
		return;
	}
	
	if (ft245dev->flags & FT245_FLAGS_RX_BUSY) {
		ft245dbg(FT245_DBG_ERROR, "ft245 RX busy");
		goto reload;
	}
	ft245dev->flags |= FT245_FLAGS_RX_BUSY;

	switch (urb->status) {
		case -EOVERFLOW:
			ft245dev->usboverruns++;
			ft245dbg(FT245_DBG_ERROR, "read data overrun");
			break;
		case 0:
			// valid data is available
			if (count > 2) {
				ft245dbg(FT245_DBG_READCALL,
				"ft245_read_bulk_callback read count %d head/cnt %d/%d (%d+%d) %02x %02x",
					count, ft245dev->rbufhead, ft245dev->rbufcnt, count/64,
					count%64, (unsigned char)ft245dev->callbackbuf[2],
					(unsigned char )ft245dev->callbackbuf[3]);
				if (count < 32)
					ft245_dump_buf("read  ", ft245dev->callbackbuf,
									count < 16 ? count : 16);
			}
			callbackndx = 0;
			while (callbackndx < count) {
				chunksize = count - callbackndx;
				if (chunksize > 64)
					chunksize = 64;
				if (chunksize < 2) {
					ft245dbg(FT245_DBG_ERROR, "ft245_read_bulk_callback short %d",
							chunksize);
					ft245_dump_buf("read ", &ft245dev->callbackbuf[callbackndx],
							chunksize);
					break;
				}
				if ((ft245dev->callbackbuf[callbackndx+0] != 0x31) ||
					((ft245dev->callbackbuf[callbackndx+1] != 0x60) &&
					 (ft245dev->callbackbuf[callbackndx+1] != 0x00))) {
					ft245dbg(FT245_DBG_ERROR,
						"2 byte header error!  May be out of sync.");
					ft245dbg(FT245_DBG_ERROR, "ft245_read_bulk_callback count %d chunksize %d flags %d status %d over %d overcnt %d usbover %d",
						count, chunksize, ft245dev->flags,
						urb->status, ft245dev->roverruns,
						ft245dev->roverruncnt,
						ft245dev->usboverruns);
					ft245_dump_buf("readerr ",
						&ft245dev->callbackbuf[callbackndx],
						chunksize < 16 ? chunksize :16);
					break;
				}
				if (chunksize == 2)
					break;
				// skip the two status bytes from the FTDI chip
				callbackndx += 2;
				chunksize -= 2;
				// we now have between 2 and 62 bytes of data
				rbuftail = ft245dev->rbufhead + ft245dev->rbufcnt;
				if (rbuftail + chunksize > READ_BUFFER_SIZE) {
					ft245dev->roverruns++;
					ft245dev->roverruncnt += chunksize;
					ft245dbg(FT245_DBG_ERROR,
						"ft245_read_bulk_callback dropped %d h/t/c %d/%d/%d", chunksize, ft245dev->rbufhead, rbuftail, ft245dev->rbufcnt);
				} else {
					memcpy(&ft245dev->rbufp[rbuftail],
						&ft245dev->callbackbuf[callbackndx],
						chunksize);
					ft245dev->rbufcnt += chunksize;
					if (ft245dev->flags & FT245_DBG_READCALL)
						ft245_dump_buf("read chunk ", &ft245dev->callbackbuf[callbackndx],
							chunksize < 16 ?  chunksize : 16);
					ft245dbg(FT245_DBG_READCALL,
						"ft245_read_bulk_callback added %d", chunksize);
				}
				callbackndx += chunksize;
				//wake_up_interruptible(&ft245dev->wait);
			}
			goto reload;
		case -ETIMEDOUT:
			ft245dbg(FT245_DBG_ERROR, "read no response");
			break;
		case -ENOENT:
			ft245dbg(FT245_DBG_ERROR, "read urb killed");
			break;
		case -ECONNRESET:
			ft245dbg(FT245_DBG_ERROR, "read connection reset");
			break;
		case -EILSEQ:
			ft245dbg(FT245_DBG_ERROR, "CRC error");
			break;
		case -EPROTO:
			ft245dbg(FT245_DBG_ERROR, "internal error");
			break;
		default:
			ft245dbg(FT245_DBG_ERROR, "ft245 RX status %d", urb->status);
			break;
	}

	ft245dev->flags &= ~FT245_FLAGS_RX_BUSY;
	ft245dev->flags |= FT245_FLAGS_DEV_ERROR;
	wake_up_interruptible(&ft245dev->wait);
	return;

reload:
	usb_fill_bulk_urb(ft245dev->rx_urb, ft245dev->udev,
		usb_rcvbulkpipe(ft245dev->udev, ft245dev->inEP),
		ft245dev->callbackbuf, ft245dev->callbackbufsize,
		ft245_read_bulk_callback, ft245dev);
	//ft245dev->rx_urb->transfer_flags |= USB_ASYNC_UNLINK;
	ft245dev->rx_urb->status = 0;
	if ((res = usb_submit_urb(ft245dev->rx_urb, GFP_ATOMIC /*GFP_KERNEL*/)))
		ft245warn("ft245 failed submit rx_urb %d", res);
	ft245dev->flags &= ~FT245_FLAGS_RX_BUSY;
	wake_up_interruptible(&ft245dev->wait);
}


static unsigned int ft245_poll(struct file *file, struct poll_table_struct *wait)
{
	struct usb_ft245	*ft245dev;
	unsigned int		rp, wp;
	static unsigned int		lastrp = 0, lastwp = 0;

	ft245dev = (struct usb_ft245 *) file->private_data;
	if (!ft245dev || !ft245dev->udev) {
		ft245dbg(FT245_DBG_ERROR, "ft245_poll NULL ft245!");
		return -ENODEV;
	}
	if (!(ft245dev->flags & FT245_FLAGS_DEV_OPEN)) {
		ft245dbg(FT245_DBG_ERROR, "ft245_poll device not open");
		return POLLERR | POLLHUP;
	}
	if (ft245dev->flags & FT245_FLAGS_DEV_ERROR) {
		ft245dbg(FT245_DBG_ERROR, "ft245_poll device error");
		return POLLERR | POLLHUP;
	}
	poll_wait(file, &ft245dev->wait, wait);
	if (!ft245dev || !ft245dev->udev) {
		ft245dbg(FT245_DBG_ERROR, "ft245_poll NULL ft245! (after wait)");
		return -ENODEV;
	}
	if (!(ft245dev->flags & FT245_FLAGS_DEV_OPEN)) {
		ft245dbg(FT245_DBG_ERROR, "ft245_poll device not open (after wait)");
		return POLLERR | POLLHUP;
	}
	if (ft245dev->flags & FT245_FLAGS_DEV_ERROR) {
		ft245dbg(FT245_DBG_ERROR, "ft245_poll device error");
		return POLLERR | POLLHUP;
	}
	if (/*!(ft245dev->flags & FT245_FLAGS_RX_BUSY) &&*/ (ft245dev->rbufcnt > 0))
		rp = POLLIN | POLLRDNORM;
	else
		rp = 0;
	wp = POLLOUT | POLLWRNORM;
	if ((lastrp != rp) || (lastwp != wp)) {
		ft245dbg(FT245_DBG_POLL, "ft245_poll rd 0x%02x wr 0x%02x", rp, wp);
		lastrp = rp;
		lastwp = wp;
	}

	return rp | wp;
}


static int ft245_release(struct inode *inode, struct file *file)
{
	struct usb_ft245	*ft245dev;

	ft245dbg(FT245_DBG_OPENCLOSE, "ft245_release");
	if (!file) {
		ft245dbg(FT245_DBG_ERROR, "Release with no file structure!");
		return 0;
	}
	ft245dev = (struct usb_ft245 *) file->private_data;
	if (!ft245dev) {
		ft245dbg(FT245_DBG_ERROR, "Release with no device structure!");
		return 0;
	}
	if (ft245dev->flags & FT245_FLAGS_PENDING_CLEANUP)
		ft245dbg(FT245_DBG_ERROR, "Release with device pending cleanup!");
	else if (!(ft245dev->flags & FT245_FLAGS_DEV_OPEN)) {
		ft245dbg(FT245_DBG_ERROR, "Release with device not open (flags)!");
		return 0;
	}
	if ((ft245dev->flags & ~FT245_FLAGS_MASK) != 0) {
		ft245dbg(FT245_DBG_ERROR, "Release corrupt flags! 0x%x", ft245dev->flags);
		return 0;
	}
	ft245dbg(FT245_DBG_OPENCLOSE, "ft245_release flags 0x%x close #%d intr cnt %d over %d overcnt %d usbover %d",
		ft245dev->flags, iminor(inode), 0 /*ft245dev->intrcnt*/,
		ft245dev->roverruns, ft245dev->roverruncnt, ft245dev->usboverruns); 

	down (&ft245dev->sem);

	if (ft245dev->flags & FT245_FLAGS_PENDING_CLEANUP) {
		ft245_cleanup(ft245dev);
	} else {
		ft245dev->flags = 0;
		usb_kill_urb(ft245dev->rx_urb);
		up (&ft245dev->sem);
	}
	

	return 0;
}


static void ft245_delete(struct kref *kref)
{	
	struct usb_ft245 *dev = container_of(kref, struct usb_ft245, kref);

	usb_put_dev(dev->udev);
	kfree (dev->rbufp);
	kfree (dev->callbackbuf);
	kfree (dev);
}


static void ft245_disconnect(struct usb_interface *interface)
{
	struct usb_ft245	*ft245dev = NULL;
	int			minor = interface->minor;

	ft245dbg(FT245_DBG_OPENCLOSE, "ft245_disconnect");
	ft245dev = usb_get_intfdata(interface);
	/* prevent skel_open() from racing skel_disconnect() */
#if HAVE_UNLOCKED_IOCTL
	mutex_lock(&ft245dev->fs_mutex);
#else
	lock_kernel();
#endif

	usb_set_intfdata(interface, NULL);

	/* give back our minor */
	usb_deregister_dev(interface, &ft245_class);

#if HAVE_UNLOCKED_IOCTL
	mutex_unlock(&ft245dev->fs_mutex);
#else
	unlock_kernel();
#endif

	/* decrement our usage count */
	kref_put(&ft245dev->kref, ft245_delete);

	ft245info("USB FT245 #%d disconnected", minor - USB_FT245_MINOR_BASE);
}


static void ft245_cleanup(struct usb_ft245 *ft245dev)
{
	usb_free_urb(ft245dev->rx_urb);
	wake_up_interruptible(&ft245dev->wait);
	kfree (ft245dev->rbufp);
	kfree (ft245dev->callbackbuf);
	kfree (ft245dev->wbufp);
	// down was called before this
	up (&ft245dev->sem);
	kfree (ft245dev);
}


static void ft245_dump_buf(unsigned char *str, unsigned char *buf, int len)
{
	int i, j, linendx;
	unsigned char linebuf[81], c;

	if (!buf)
		return;

	for (i = 0; i < len; i += 16) {
		linendx = 0;
		for (j = 0; (j < 16) && (i+j < len); j++) {
			c = buf[i+j];
			if (((c>>4) & 0x0f) > 9)
				linebuf[linendx++] = ((c>>4) & 0x0f)-10 + 'a';
			else
				linebuf[linendx++] = ((c>>4) & 0x0f) + '0';
			if ((c & 0x0f) > 9)
				linebuf[linendx++] = (c & 0x0f)-10 + 'a';
			else
				linebuf[linendx++] = (c & 0x0f) + '0';
			linebuf[linendx++] = ' ';
		}
		for (; j < 16; j++) {
			linebuf[linendx++] = ' ';
			linebuf[linendx++] = ' ';
			linebuf[linendx++] = ' ';
		}
		linebuf[linendx++] = ' ';
		linebuf[linendx++] = '|';
		for (j = 0; (j < 16) && (i+j < len); j++) {
			c = buf[i+j];
			if ((c >= '!') && (c <= '~'))
				linebuf[linendx++] = c;
			else
				linebuf[linendx++] = '.';
		}
		for (; j < 16; j++)
			linebuf[linendx++] = ' ';
		linebuf[linendx++] = '|';
		linebuf[linendx++] = '\0';
		ft245dbg((FT245_DBG_WRITE|FT245_DBG_CONTROL|FT245_DBG_READCALL), "%s%s", str, linebuf);
	}
}


int __init usb_ft245_init(void)
{
	ft245dbg(FT245_DBG_INIT, "FT245 driver init (debug=0x%02x)", debug);
 	if (usb_register (&ft245_driver) < 0)
 		return -1;
	ft245info(DRIVER_VERSION ":" DRIVER_DESC);
	return 0;
}


void __exit usb_ft245_cleanup(void)
{
	ft245dbg(FT245_DBG_INIT, "FT245 driver exit");
	usb_deregister (&ft245_driver);
}


module_init (usb_ft245_init);
module_exit (usb_ft245_cleanup);

MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("GPL");

