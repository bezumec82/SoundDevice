#include "i2s_mod.h"
/*! DMA configuration routine
 */
 
static void i2s_config_dma_slave (struct i2s_device * i2s_dev)
{
    memset(&i2s_dev->dma.slave_config, 0, sizeof(i2s_dev->dma.slave_config));
    /*! -# Provide the physical address where DMA slave data should be read(RX)/write(TX). */
    i2s_dev->dma.slave_config.src_addr = (dma_addr_t)i2s_dev->regs->start + I2SC_RHR_OFF; //RX
    i2s_dev->dma.slave_config.dst_addr = (dma_addr_t)i2s_dev->regs->start + I2SC_THR_OFF; //TX

    /*! -# Provide width in bytes of the source (RX)/(TX) register where DMA data shall be read/write. */
    i2s_dev->dma.slave_config.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES; //RX
    i2s_dev->dma.slave_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES; //TX

    /*! -# Set the maximum number of words that can be sent/receive in one burst to/from the device.
     * (note: words, as in units of the src_addr_width member, not bytes) */
    i2s_dev->dma.slave_config.src_maxburst = 1;
    i2s_dev->dma.slave_config.dst_maxburst = 1;
    /*! -# Flow Controller Settings. Only valid for slave channels.
     * Fill with 'true' if peripheral should be flow controller.
     * Direction will be selected at Runtime.*/
	i2s_dev->dma.slave_config.device_fc = false;
}
/*--- End of function ----------------------------------------------------------------------------*/

static int i2s_configure_dma_channels (struct i2s_device * i2s_dev)
{
    int ret_val = 0;
    /* try to allocate an exclusive slave channel */
    i2s_dev->dma.tx_ch = dma_request_slave_channel_reason   (
                                                            i2s_dev->dev,   //struct device
                                                            "tx"            //slave channel name
                                                            );
	if (IS_ERR(i2s_dev->dma.tx_ch))
    {
        __PRINTK(RED, "DMA channel TX is not availible.\n");
		ret_val = PTR_ERR(i2s_dev->dma.tx_ch);
		i2s_dev->dma.tx_ch = NULL;
		return ret_val;
	}
	i2s_dev->dma.rx_ch = dma_request_slave_channel_reason(i2s_dev->dev, "rx");
	if (IS_ERR(i2s_dev->dma.rx_ch)) 
    {
        __PRINTK(RED, "DMA channel RX is not availible.\n");
		ret_val = PTR_ERR(i2s_dev->dma.rx_ch);
		i2s_dev->dma.rx_ch = NULL;
		return ret_val;
	}
    i2s_dev->dma.slave_config.direction = DMA_MEM_TO_DEV;
	if (dmaengine_slave_config(i2s_dev->dma.tx_ch, &i2s_dev->dma.slave_config)) 
    {
		__PRINTK(RED, "Failed to configure TX channel.\n");
		ret_val = -EINVAL;
		return ret_val;
	}
	i2s_dev->dma.slave_config.direction = DMA_DEV_TO_MEM;
	if (dmaengine_slave_config(i2s_dev->dma.rx_ch, &i2s_dev->dma.slave_config)) 
    {
		__PRINTK(RED, "Failed to configure RX channel.\n");
		ret_val = -EINVAL;
		return ret_val;
	}
    return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

#if DMA_SIMPLE
int i2s_configure_dma (struct i2s_device * i2s_dev)
{
    int ret_val = 0;
    __PRINTK(YEL, "Configuring DMA for I2S module.\n");
    i2s_config_dma_slave (i2s_dev);
    ret_val = i2s_configure_dma_channels (i2s_dev);
    if (ret_val < 0 )
    {
        __PRINTK(RED,"Can't configure DMA channels. %i returned.\n", ret_val); 
        goto error; 
    }      
	__PRINTK(GRN,   "Using '%s' (TX) and '%s' (RX) for DMA transfers.\n",
                    dma_chan_name(i2s_dev->dma.tx_ch), dma_chan_name(i2s_dev->dma.rx_ch));                    
    return ret_val;
error:
    __PRINTK(RED, "Release DMA channels.\n");
	if (i2s_dev->dma.rx_ch) dma_release_channel(i2s_dev->dma.rx_ch);
	if (i2s_dev->dma.tx_ch) dma_release_channel(i2s_dev->dma.tx_ch);
	return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

void i2s_disable_dma (struct i2s_device * i2s_dev)
{
    __PRINTK(RED, "Disable DMA for controller '%s'.\n", i2s_dev->plat_data->dev_name);
    if (i2s_dev->dma.rx_ch) dma_release_channel(i2s_dev->dma.rx_ch);
	if (i2s_dev->dma.tx_ch) dma_release_channel(i2s_dev->dma.tx_ch);
}
/*--- End of function ----------------------------------------------------------------------------*/

struct dma_data {
    struct device *                     dev;
    void *                              buf;
    struct dma_async_tx_descriptor *    desc;
    dma_addr_t                          addr;
    size_t                              count;
    dma_cookie_t                        cookie;
};

static void i2s_write_data_dma_callback(void * data)
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

ssize_t i2s_write   (
                    struct file *       filp, 
                    const char __user * buf, 
                    size_t              count,
                    loff_t *            f_pos
                    )
{
    ssize_t                             ret_val     = 0;
    struct dma_data *                   dma         = NULL;
    struct i2s_device *                 i2s_dev     = filp->private_data;
    
#if DEBUG_DMA
    __PRINTK(CYN, "Writing device %s in dma scat.-gather mode.\n", i2s_dev->plat_data->dev_name);
#endif
    
    if (mutex_lock_interruptible(&i2s_dev->mutex)) { return -ERESTARTSYS; }
        /*! Allocate facility */
        dma = (struct dma_data *)kmalloc(sizeof(struct dma_data), GFP_KERNEL);
        if (!dma) { goto out_of_mem; }
        /*! Allocate memory */
        dma->buf = (void *)__get_free_pages(GFP_KERNEL, 0);
        if (!dma->buf) { goto out_of_mem; }        
        dma->dev = i2s_dev->dev;        
        /*! Get data from user */
        if (count%4) { count += (4 - (count%4)); } /* align to 4 bytes */    
        count > PAGE_SIZE ? /* for allign */
                            (dma->count = PAGE_SIZE) : (dma->count = count);        
        if ( copy_from_user( dma->buf, buf, dma->count ) )
        {
            __PRINTK(RED, "ERROR : Failed to get data from user.\n");
            ret_val = -EFAULT; goto cpy_err;
        }
        #if DEBUG_DMA
                __PRINTK(CYN, "%u bytes copied from user.\n", dma->count);
        #endif
        /*! Map buffer */
        dma->addr = dma_map_single   (
                                    dma->dev, 
                                    dma->buf, 
                                    dma->count,
                                    DMA_TO_DEVICE);                                
        if (dma_mapping_error(dma->dev, dma->addr))
        {
            __PRINTK(RED, "ERROR : Failed to map DMA.\n");
            ret_val = -EFAULT;
            goto dma_map_err;
        }
        /*! Prepare slave */
        dma->desc = dmaengine_prep_slave_single    (
                                                    i2s_dev->dma.tx_ch, 
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
        dma->desc->callback        = i2s_write_data_dma_callback;
        dma->desc->callback_param  = dma;
        
        /*! Push current DMA TX transaction in the pending queue */
        dma->cookie = dmaengine_submit(dma->desc);
        dma_async_issue_pending(i2s_dev->dma.tx_ch);
        
        *f_pos += dma->count;
        ret_val = dma->count;
    
    mutex_unlock(&i2s_dev->mutex);
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
    mutex_unlock(&i2s_dev->mutex);
    return ret_val;

out_of_mem:
    __PRINTK(RED,"Can't allocate memory.\n"); 
cpy_err:
    if (dma) { kfree(dma); }
    ret_val = -ENOMEM; 
    mutex_unlock(&i2s_dev->mutex);
    return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

#endif

#if DMA_CIRC_BUF
static void inline word_set(int * array, int value, int count)
{
    while(count--)
        *array++ = value;
}

static void i2s_write_data_dma_callback(void * data)
{
    struct i2s_device * i2s_dev = (struct i2s_device *)data;
#if (DEBUG_DMA_TAIL)
    char *              tx_buf  = i2s_dev->dma.tx_ring.buf;
    int                 tail; 
#endif
    spin_lock(&i2s_dev->dma.lock);
        i2s_dev->dma.dma_started = 1;
        i2s_dev->dma.tx_ring.tail += DMA_BUF_PERIOD;
        if (i2s_dev->dma.tx_ring.tail >= DMA_BUF_SIZE)
        {
            i2s_dev->dma.tx_ring.tail = i2s_dev->dma.tx_ring.tail - DMA_BUF_SIZE;
        }
        i2s_dev->dma.tasklet_counter++;
#if (DEBUG_DMA_TAIL)
        tail = i2s_dev->dma.tx_ring.tail;
        if (tail <= DMA_BUF_SIZE - DMA_BUF_PERIOD)
        {
            *(int *)&tx_buf[tail + DMA_BUF_PERIOD - 22] = i2s_dev->dma.tx_ring.tail/DMA_BUF_PERIOD;
            *(int *)&tx_buf[tail + DMA_BUF_PERIOD - 18] = i2s_dev->dma.tx_ring.head/DMA_BUF_PERIOD;
        }
//        if (tail > DMA_BUF_SIZE/2)
//        {
//            *(int *)&tx_buf[tail - DMA_BUF_PERIOD/2 + 2] = 0xffffffff;
//            *(int *)&tx_buf[tail - DMA_BUF_PERIOD/2 + 6] = 0xaaaaaaaa;
//        }
#endif
    spin_unlock(&i2s_dev->dma.lock);
    return ;
}
/*--- End of function ----------------------------------------------------------------------------*/


static void clean_tasklet_callback(unsigned long data)
{
    struct i2s_device * i2s_dev     = (struct i2s_device *)data;
    char *              tx_buf      = i2s_dev->dma.tx_ring.buf;  
    int                 prev_tail;     
    int                 curr_tail;
    int                 residue;
    
    /* Read current tail state. */ 
    spin_lock(&i2s_dev->dma.lock);
        prev_tail   = i2s_dev->dma.prev_tx_tail;  
        curr_tail   = i2s_dev->dma.tx_ring.tail;
        if (curr_tail <= DMA_BUF_SIZE - DMA_BUF_PERIOD)
        {
            *(int *)&tx_buf[curr_tail + DMA_BUF_PERIOD - 14] = curr_tail/DMA_BUF_PERIOD;
            *(int *)&tx_buf[curr_tail + DMA_BUF_PERIOD - 10] = prev_tail/DMA_BUF_PERIOD;
        }
    spin_unlock(&i2s_dev->dma.lock);

    if (curr_tail > prev_tail) /* Tail is moved further. */
    {
        word_set((int *)&tx_buf[prev_tail], 0x00000000, (curr_tail - prev_tail)/4);
        //memset(&tx_buf[prev_tail], 0x00, (curr_tail - prev_tail));
    }
    
    if (prev_tail > curr_tail) /* Overflow happend. Clean in two steps. */
    {
        residue = DMA_BUF_SIZE - prev_tail;
        if (residue) word_set((int *)&tx_buf[prev_tail], 0x00000000, residue/4);
        word_set((int *)&tx_buf[0], 0x00000000, curr_tail/4);
        //if(residue) memset(&tx_buf[prev_tail], 0x00, residue);
        //memset(&tx_buf[0], 0x00, curr_tail);
    }
    
    spin_lock(&i2s_dev->dma.lock);
        i2s_dev->dma.prev_tx_tail = curr_tail; /* Save new tail state. */
    spin_unlock(&i2s_dev->dma.lock);
    
    /* Reschedule self. */
    if (!atomic_read(&i2s_dev->dma.shutdown))
        tasklet_schedule(&i2s_dev->dma.clean_tasklet);
}
/*--- End of function ----------------------------------------------------------------------------*/

static void stat_timer_callback(u_long data) 
{
    struct i2s_device * i2s_dev     = (struct i2s_device *)data;
    struct timespec     time;
    u32                 curr_time;
    u32                 tasklet_counter;
    u32                 rate;
    u32                 tasklet_delta;
    /// -# Change the expiration time of an already scheduled.
    if (!atomic_read(&i2s_dev->dma.shutdown))    
        mod_timer(&i2s_dev->dma.stat_timer, jiffies + STAT_INTERVAL);
        
    if (i2s_dev->dma.dma_started){
        spin_lock(&i2s_dev->dma.lock);
            tasklet_counter = i2s_dev->dma.tasklet_counter;
        spin_unlock(&i2s_dev->dma.lock);
        
        time        = current_kernel_time();
        curr_time   = (u32)(time.tv_nsec/1000000);
        i2s_dev->dma.necessary_task_count   += (STAT_INTERVAL * BYTES_PER_MS) / DMA_BUF_PERIOD;
        tasklet_delta   = tasklet_counter - i2s_dev->dma.prev_task_count;        
        rate = (u32)((curr_time - i2s_dev->dma.prev_stat_time)/tasklet_delta);        
        printk( "%ums : t.cnt%i : %ims/t : n.cnt%i : delta%i\n", 
                curr_time, tasklet_counter, rate, i2s_dev->dma.necessary_task_count, tasklet_delta);

        i2s_dev->dma.prev_stat_time         = (u32)(time.tv_nsec/1000000);
        i2s_dev->dma.prev_task_count        = i2s_dev->dma.tasklet_counter;        
    }
}
/*--- End of function ----------------------------------------------------------------------------*/

int i2s_configure_dma (struct i2s_device * i2s_dev)
{
    int ret_val = 0;    
    __PRINTK(YEL,   "Configuring DMA for controller '%s'.\n", 
                    i2s_dev->plat_data->dev_name);
    i2s_config_dma_slave (i2s_dev);
    ret_val = i2s_configure_dma_channels (i2s_dev);
    if (ret_val < 0 )
    { 
        __PRINTK(RED,"Can't configure DMA channels. %i returned.\n", ret_val); 
        goto ch_error; 
    }
    i2s_dev->dma.tx_ring.buf = (char *)dma_alloc_coherent   (
                                                            i2s_dev->dev, 
                                                            DMA_BUF_SIZE,
                                                            &i2s_dev->dma.tx_hdl,
                                                            GFP_KERNEL
                                                            );
    if (!i2s_dev->dma.tx_ring.buf) {
        __PRINTK(RED,"Can't allocate memory.\n"); 
        ret_val = -ENOMEM; 
		goto ch_error;
	}
    /* Prepare a DMA cyclic transaction */
	i2s_dev->dma.tx_desc = dmaengine_prep_dma_cyclic(
                                                    i2s_dev->dma.tx_ch,
                                                    i2s_dev->dma.tx_hdl,    //buffer address
                                                    DMA_BUF_SIZE,           //buffer length
                                                    DMA_BUF_PERIOD,         //callback period
                                                    DMA_MEM_TO_DEV,
                                                    DMA_PREP_INTERRUPT
                                                    );
    if (!i2s_dev->dma.tx_desc) 
    {
		__PRINTK(RED, "'dmaengine_prep_dma_cyclic' for TX failed\n");
		ret_val = -ENODEV;
		goto cfg_error;
	}
    
    spin_lock_init(&i2s_dev->dma.lock);
    atomic_set(&i2s_dev->dma.shutdown, 0);
    tasklet_init(&i2s_dev->dma.clean_tasklet, clean_tasklet_callback, (unsigned long)i2s_dev);
    tasklet_schedule(&i2s_dev->dma.clean_tasklet);
    
    init_timer(&i2s_dev->dma.stat_timer);
    i2s_dev->dma.stat_timer.function    = stat_timer_callback;
    i2s_dev->dma.stat_timer.data        = (unsigned long)i2s_dev;
    i2s_dev->dma.stat_timer.expires     = jiffies + STAT_INTERVAL;
    add_timer(&i2s_dev->dma.stat_timer);
    
    i2s_dev->dma.tx_desc->callback          = i2s_write_data_dma_callback;
	i2s_dev->dma.tx_desc->callback_param    = i2s_dev;
    i2s_dev->dma.tx_cookie                  = dmaengine_submit(i2s_dev->dma.tx_desc);
    ret_val                                 = dma_submit_error(i2s_dev->dma.tx_cookie);
    if (ret_val)
    {
        __PRINTK(RED, "'dmaengine_submit' for TX failed\n");
		goto sbmt_error;
    }   

    dma_async_issue_pending(i2s_dev->dma.tx_ch);
	__PRINTK(GRN,   "Using '%s/%i/%i' (TX) and '%s/%i/%i' (RX) for DMA transfers.\n",
                    dma_chan_name(i2s_dev->dma.tx_ch), i2s_dev->dma.tx_ch->chan_id, i2s_dev->dma.tx_ch->device->dev_id,
                    dma_chan_name(i2s_dev->dma.rx_ch), i2s_dev->dma.rx_ch->chan_id, i2s_dev->dma.rx_ch->device->dev_id);
    return ret_val;

sbmt_error:
    dmaengine_terminate_async(i2s_dev->dma.tx_ch);

cfg_error:
    dma_free_coherent   (
                        i2s_dev->dev, 
                        DMA_BUF_SIZE,
                        i2s_dev->dma.tx_ring.buf,
                        i2s_dev->dma.tx_hdl
                        );
ch_error:
    __PRINTK(RED, "Release DMA channels.\n");
	if (i2s_dev->dma.rx_ch) dma_release_channel(i2s_dev->dma.rx_ch);
	if (i2s_dev->dma.tx_ch) dma_release_channel(i2s_dev->dma.tx_ch);
	return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

void i2s_disable_dma (struct i2s_device * i2s_dev)
{
    __PRINTK(RED, "Disable DMA for controller '%s'.\n", i2s_dev->plat_data->dev_name);
    atomic_inc(&i2s_dev->dma.shutdown);
    tasklet_kill(&i2s_dev->dma.clean_tasklet);
    del_timer(&i2s_dev->dma.stat_timer);
    if (i2s_dev->dma.tx_ch)
    {        
        dmaengine_terminate_sync(i2s_dev->dma.tx_ch);
    }
    spin_unlock(&i2s_dev->dma.lock);

    dma_free_coherent   (
                        i2s_dev->dev, 
                        DMA_BUF_SIZE,
                        i2s_dev->dma.tx_ring.buf,
                        i2s_dev->dma.tx_hdl
                        );
    if (i2s_dev->dma.rx_ch) dma_release_channel(i2s_dev->dma.rx_ch);
    if (i2s_dev->dma.tx_ch) dma_release_channel(i2s_dev->dma.tx_ch);
}
/*--- End of function ----------------------------------------------------------------------------*/

void read_CSA (struct i2s_device * i2s_dev){
    void __iomem * base;
    if (!i2s_dev->dma.tx_ch->device->dev_id)
    {
        base = ioremap(XDMAC_CSA_D0_CH0, XDMAC_CSA_D0_CH15 - XDMAC_CSA_D0_CH0 + 4);
        __PRINTK(RED, "%u\n",
        __raw_readl(base + XDMAC_CSA_STEP * i2s_dev->dma.tx_ch->chan_id));
    }
    if (i2s_dev->dma.tx_ch->device->dev_id == 1)
    {
        base = ioremap(XDMAC_CSA_D1_CH0, XDMAC_CSA_D1_CH15 - XDMAC_CSA_D1_CH0 + 4);
        __PRINTK(RED, "%u\n",
        __raw_readl(base + XDMAC_CSA_STEP * i2s_dev->dma.tx_ch->chan_id));
    }
}
/*--- End of function ----------------------------------------------------------------------------*/

ssize_t i2s_write   (
                    struct file *       filp, 
                    const char __user * usr_buf, 
                    size_t              count,
                    loff_t *            f_pos
                    )
{
    ssize_t                             ret_val     = 0;
    struct i2s_device *                 i2s_dev     = filp->private_data;    
    int                                 curr_capacity;      
    int                                 head;
    int                                 tail;
    size_t                              usr_count;
    char *                              tx_buf      = i2s_dev->dma.tx_ring.buf;
    size_t                              cnt_to_end;
    
#if ( DEBUG_DEV )
    __PRINTK(CYN, "Writing device %s in dma circ. buf. mode.\n", i2s_dev->plat_data->dev_name);
#endif
    
    if (mutex_lock_interruptible(&i2s_dev->mutex)) { return -ERESTARTSYS; }
    
    /* Make the screenshoot of the current circ. buf. state */
    head        = i2s_dev->dma.tx_ring.head;
    spin_lock(&i2s_dev->dma.lock);    
        tail        = i2s_dev->dma.tx_ring.tail;
    spin_unlock(&i2s_dev->dma.lock);
    

    //read_CSA(i2s_dev);
    
    
    /* 
     * First place head before 'tail'. 
     * 'tail' moves continuously from the module insertion moment. 
     * It is impossible to control its position.
     */
    if (head < tail + DMA_BUF_HEAD_OUTRUN) /* If 'head' is falling behind 'tail'. */
    {
        if ( tail >= DMA_BUF_SIZE - DMA_BUF_HEAD_MAX_OUTRUN ) /* 'tail' is at the end of the buffer. */
        {
            if (head > DMA_BUF_HEAD_MAX_OUTRUN - (DMA_BUF_SIZE - tail)) /* 'head' at the beginning and too far from 'tail'. */
            {
                printk(YEL"h=%d/t=%d:", (int)(head/BYTES_PER_MS), (int)(tail/BYTES_PER_MS));
                head = tail + DMA_BUF_HEAD_OUTRUN;
                if (head >= DMA_BUF_SIZE) { head -= DMA_BUF_SIZE; }
                printk("%d<-h\n"NORM, (int)(head/BYTES_PER_MS));
            } /* 'else' is the sole good exodus from situation. */
        } else { 
            printk(YEL"h=%d/t=%d:", (int)(head/BYTES_PER_MS), (int)(tail/BYTES_PER_MS));
            head = tail + DMA_BUF_HEAD_OUTRUN; /* Moving 'head' before 'tail'. */
            if (head >= DMA_BUF_SIZE) { head -= DMA_BUF_SIZE; }
            printk("%d<-h\n"NORM, (int)(head/BYTES_PER_MS));
        }        
    } //end if (head < tail)
     
    else if (head > tail + DMA_BUF_HEAD_MAX_OUTRUN) /* if 'head' too far from 'tail'. */
    {
        printk(YEL"h=%d/t=%d:", (int)(head/BYTES_PER_MS), (int)(tail/BYTES_PER_MS));
        head = tail + DMA_BUF_HEAD_OUTRUN; /* Return 'head' back. */        
        printk("%d<-h\n"NORM, (int)(head/BYTES_PER_MS));
    }    
    i2s_dev->dma.tx_ring.head = head; /* Save new 'head'. */
    
    
    
    /* Measure the remaining capacity of circ. buf. */
    curr_capacity = CIRC_SPACE(head, tail, DMA_BUF_SIZE);
    if (curr_capacity < DMA_BUF_QUANTUM ) /* Impossible condition. */
    {
        __PRINTK(RED,   "!!! IMPOSSIBLE !!! h=%d/t=%d/cap=%d/q=%lu\n", 
                        head, tail, curr_capacity, DMA_BUF_SIZE);
        return -ERESTARTSYS;
    }
    
    
    
    /* Align to 4 bytes. */
    if (count%4)        { count -= count%4; }              
    count > DMA_BUF_QUANTUM ?
                        (usr_count = DMA_BUF_QUANTUM) : (usr_count = count);    
    if (!count || !usr_count) goto empty;        
    cnt_to_end  = DMA_BUF_SIZE - head; /* How many space until the end of circ. buf. */

    if ( usr_count < cnt_to_end ) /* Copy without overflow. */
    {
#if ( DEBUG_DMA_CPY_USR )
        printk( CYN"h=%d/->e=%d/cap=%d/u<-%d\n", 
                (int)(head/DMA_BUF_PERIOD), cnt_to_end, curr_capacity, usr_count);
#endif
        if (copy_from_user(&tx_buf[head], &usr_buf[0], usr_count)) { COPY_FROM_USR_ERR }
    }
    else if ( usr_count >= cnt_to_end ) /* Copy with overflow in two steps. */
    {
        if (cnt_to_end)
        {
#if ( DEBUG_DMA_CPY_USR )
        printk( YEL"1)h=%d/->e=%d/%d<-u\n"NORM, 
                (int)(head/DMA_BUF_PERIOD), cnt_to_end, cnt_to_end);
#endif
            if (copy_from_user (&tx_buf[head], &usr_buf[0], cnt_to_end)) { COPY_FROM_USR_ERR }
        }

        if (usr_count - cnt_to_end)
        {
#if ( DEBUG_DMA_CPY_USR )
        printk( YEL"2)h=%d/->e=%d/%i<-u\n"NORM, 
                (int)(head/DMA_BUF_PERIOD), cnt_to_end, (usr_count - cnt_to_end));
#endif
            if (copy_from_user  (
                                &tx_buf[0],
                                &usr_buf[cnt_to_end],
                                (usr_count - cnt_to_end)
                                )
                                )  { COPY_FROM_USR_ERR }
        }
    }
    
    i2s_dev->dma.tx_ring.head += usr_count;
    if (i2s_dev->dma.tx_ring.head >= DMA_BUF_SIZE) { i2s_dev->dma.tx_ring.head -= DMA_BUF_SIZE; }
    
empty:     
    *f_pos += usr_count;
    ret_val = usr_count;
out:
    mutex_unlock(&i2s_dev->mutex);
    return ret_val;
}
/*--- End of function ----------------------------------------------------------------------------*/

#endif


