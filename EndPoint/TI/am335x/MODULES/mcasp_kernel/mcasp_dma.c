#include "mcasp_mod.h"
/*! DMA configuration routine
 */

static void mcasp_config_dma_slave (struct mcasp_device * mcasp_dev)
{
	memset(&mcasp_dev->dma.slave_config, 0, sizeof(mcasp_dev->dma.slave_config));
	/*! -# Provide the physical address where DMA slave data should be read(RX)/write(TX). */
	mcasp_dev->dma.slave_config.src_addr = (dma_addr_t)mcasp_dev->dat_regs->start; //RX
	mcasp_dev->dma.slave_config.dst_addr = (dma_addr_t)mcasp_dev->dat_regs->start; //TX

	/*! -# Provide width in bytes of the source (RX)/(TX) register where DMA data shall be read/write. */
	mcasp_dev->dma.slave_config.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES; //RX
	mcasp_dev->dma.slave_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES; //TX

	/*! -# Set the maximum number of words that can be sent/receive in one burst to/from the device.
	* (note: words, as in units of the src_addr_width member, not bytes) */
	mcasp_dev->dma.slave_config.src_maxburst = 2; /* Two serializers. */
	mcasp_dev->dma.slave_config.dst_maxburst = 2;
	/*! -# Flow Controller Settings. Only valid for slave channels.
	* Fill with 'true' if peripheral should be flow controller.
	* Direction will be selected at Runtime.*/
	mcasp_dev->dma.slave_config.device_fc = false;
}
/*--- End of function ----------------------------------------------------------------------------*/

static int mcasp_configure_dma_channels (struct mcasp_device * mcasp_dev)
{
	int ret_val = 0;
	/* try to allocate an exclusive slave channel */
	mcasp_dev->dma.tx_ch = dma_request_slave_channel_reason	(
															mcasp_dev->dev, //struct device
															"tx"			//slave channel name
															);
	if (IS_ERR(mcasp_dev->dma.tx_ch))
	{
		__PRINTK(RED, "DMA channel TX is not availible.\n");
		ret_val = PTR_ERR(mcasp_dev->dma.tx_ch);
		mcasp_dev->dma.tx_ch = NULL;
		return ret_val;
	}
	mcasp_dev->dma.rx_ch = dma_request_slave_channel_reason(mcasp_dev->dev, "rx");
	if (IS_ERR(mcasp_dev->dma.rx_ch))
	{
		__PRINTK(RED, "DMA channel RX is not availible.\n");
		ret_val = PTR_ERR(mcasp_dev->dma.rx_ch);
		mcasp_dev->dma.rx_ch = NULL;
		return ret_val;
	}
	mcasp_dev->dma.slave_config.direction = DMA_MEM_TO_DEV;
	if (dmaengine_slave_config(mcasp_dev->dma.tx_ch, &mcasp_dev->dma.slave_config))
	{
		__PRINTK(RED, "Failed to configure TX channel.\n");
		ret_val = -EINVAL;
		return ret_val;
	}
	mcasp_dev->dma.slave_config.direction = DMA_DEV_TO_MEM;
	if (dmaengine_slave_config(mcasp_dev->dma.rx_ch, &mcasp_dev->dma.slave_config))
	{
		__PRINTK(RED, "Failed to configure RX channel.\n");
		ret_val = -EINVAL;
		return ret_val;
	}
	return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

int mcasp_configure_dma (struct mcasp_device * mcasp_dev)
{
	int ret_val = 0;
	__PRINTK(YEL, "Configuring DMA for '%s' module.\n", mcasp_dev->plat_data->dev_name);
	mcasp_config_dma_slave (mcasp_dev);
	ret_val = mcasp_configure_dma_channels (mcasp_dev);
	if (ret_val < 0 )
	{
		__PRINTK(RED, "Can't configure DMA channels. %i returned.\n", ret_val);
		goto error;
	}
	__PRINTK(GRN,	"Using '%s' (TX) and '%s' (RX) for DMA transfers.\n",
					dma_chan_name(mcasp_dev->dma.tx_ch), dma_chan_name(mcasp_dev->dma.rx_ch));
	return ret_val;
error:
	__PRINTK(RED, "Release DMA channels.\n");
	if (mcasp_dev->dma.rx_ch) dma_release_channel(mcasp_dev->dma.rx_ch);
	if (mcasp_dev->dma.tx_ch) dma_release_channel(mcasp_dev->dma.tx_ch);
	return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

void mcasp_disable_dma (struct mcasp_device * mcasp_dev)
{
	__PRINTK(RED, "Disable DMA for controller '%s'.\n", mcasp_dev->plat_data->dev_name);
	if (mcasp_dev->dma.rx_ch) dma_release_channel(mcasp_dev->dma.rx_ch);
	if (mcasp_dev->dma.tx_ch) dma_release_channel(mcasp_dev->dma.tx_ch);
}
/*--- End of function ----------------------------------------------------------------------------*/

struct dma_data {
	struct device *						dev;
	void *								buf;
	struct dma_async_tx_descriptor *	desc;
	dma_addr_t							addr;
	size_t								count;
	dma_cookie_t						cookie;
};

static void mcasp_write_data_dma_callback(void * data)
{
	struct dma_data * dma = (struct dma_data *)data;
#if DEBUG_DMA
	__PRINTK(CYN, "DMA callback.\n");
#endif
	dma_unmap_single(
					dma->dev,
					dma->addr,
					dma->count,
					DMA_TO_DEVICE
					);
	free_page((unsigned long)dma->buf);
	kfree(dma);
}
/*--- End of function ----------------------------------------------------------------------------*/

ssize_t mcasp_write	(
					struct file *		filp,
					const char __user * buf,
					size_t				count,
					loff_t *			f_pos
					)
{
	ssize_t								ret_val		= 0;
	struct dma_data *					dma			= NULL;
	struct mcasp_device *				mcasp_dev	= filp->private_data;
#if DEBUG_DMA
	__PRINTK(CYN,	"Writing device %s in DMA scat.-gather mode.\n",
					mcasp_dev->plat_data->dev_name);
#endif
	if (mutex_lock_interruptible(&mcasp_dev->mutex)) { return -ERESTARTSYS; }
		/*! Allocate facility */
		dma = (struct dma_data *)kmalloc(sizeof(struct dma_data), GFP_KERNEL);
		if (!dma) { goto out_of_mem; }
		/*! Allocate memory */
		dma->buf = (void *)get_zeroed_page(GFP_KERNEL);
		if (!dma->buf) { goto out_of_mem; }
		dma->dev = mcasp_dev->dev;
		/*! Get data from user */
		if (count%4) { count += 4 - count%4; } /* align to 4 bytes */
		count > PAGE_SIZE ? (dma->count = PAGE_SIZE) : (dma->count = count);

		if (copy_from_user(dma->buf, buf, dma->count))
		{
			__PRINTK(RED, "ERROR : Failed to get data from user.\n");
			ret_val = -EFAULT; goto cpy_err;
		}
#if DEBUG_DMA
	__PRINTK(CYN, "%u bytes copied from user.\n", dma->count);
#endif
		/*! Map buffer */
		dma->addr = dma_map_single	(
									dma->dev,
									dma->buf,
									dma->count,
									DMA_TO_DEVICE
									);
		if (dma_mapping_error(dma->dev, dma->addr))
		{
			__PRINTK(RED, "ERROR : Failed to map DMA.\n");
			ret_val = -EFAULT;
			goto dma_map_err;
		}
		/*! Prepare slave */
		dma->desc = dmaengine_prep_slave_single		(
													mcasp_dev->dma.tx_ch,
													dma->addr,
													dma->count,
													DMA_MEM_TO_DEV,
													DMA_PREP_INTERRUPT
													);
		if (!dma->desc)
		{
			__PRINTK(RED, "ERROR : Failed to prepare DMA slave.\n");
			ret_val = -EFAULT;
			goto dma_prep_err;
		}
		dma->desc->callback			= mcasp_write_data_dma_callback;
		dma->desc->callback_param	= dma;
		/*! Push current DMA TX transaction in the pending queue */
		dma->cookie = dmaengine_submit(dma->desc);
		dma_async_issue_pending(mcasp_dev->dma.tx_ch);
		
		*f_pos += dma->count;
		ret_val = dma->count;
		
	mutex_unlock(&mcasp_dev->mutex);
	return ret_val;

dma_prep_err:
	dma_unmap_single(
					dma->dev,
					dma->addr,
					dma->count,
					DMA_TO_DEVICE
					);
dma_map_err:
	free_page((unsigned long)dma->buf);
	kfree(dma);
	mutex_unlock(&mcasp_dev->mutex);
	return ret_val;

out_of_mem:
	__PRINTK(RED,"Can't allocate memory.\n");
cpy_err:
	if (dma) { kfree(dma); }
	ret_val = -ENOMEM;
	mutex_unlock(&mcasp_dev->mutex);
	return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/


