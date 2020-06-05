#include "mcasp_rpmsg_mod.h"

#define READ_TIMEOUT_MS    20
#if DEBUG_W_BLOCK
    #define READ_TIMEOUT_JFFS   (HZ * 100 / 1000 )
#else
    #define READ_TIMEOUT_JFFS   (HZ * READ_TIMEOUT_MS / 1000 )
#endif

#define WRITE_TIMEOUT_MS    20
#if DEBUG_W_BLOCK
    #define WRITE_TIMEOUT_JFFS  (HZ * 100 / 1000 )
#else
    #define WRITE_TIMEOUT_JFFS  (HZ * WRITE_TIMEOUT_MS / 1000 )
#endif


unsigned int pru_mcasp_poll (
                            struct file *               filp,
                            struct poll_table_struct *  pts
                            )
{
    unsigned int        mask        = 0;
    struct ts_data *    ts_data_ptr = (struct ts_data *)filp->private_data;
        poll_wait(filp, &ts_data_ptr->write_q,  pts);
        poll_wait(filp, &ts_data_ptr->read_q,   pts);
        if (ts_data_ptr->can_write == 1)
        {
            mask |= POLLOUT | POLLWRNORM;
        }
        if (ts_data_ptr->can_read == 1)
        {
            mask |= POLLIN | POLLRDNORM;
        }        
    PRINTK_COND(DEBUG_POLL, CYN, "ts=%i, m=%i f=%i.\n", ts_data_ptr->ts_idx, mask, ts_data_ptr->can_write);
    return mask;
}
/*--- End of the function ------------------------------------------------------------------------*/

int read_data_quantum   (
                        struct pru_mcasp_device *   pru_mcasp_dev_ptr,  /* Provides data about buffers. */
                        void *                      data,               /* Data to copy. */
                        uint32_t                    byte_count,         /* How many data bytes to write. */
                        int                         ts_idx              /* Timeslot to use. */
                        )
{
    //int32_t             rx_tail  = 0; ///< Index of the buf in time slot bufs array to START READING
    struct ts_data *    ts_data_ptr = &pru_mcasp_dev_ptr->ts_data[ts_idx];
    phys_addr_t         page_addr   = 0;
    struct page *       page_ptr    = NULL;
    int                 ret_val     = 0;
    
    /*! Define position where start to read. 
     * It should be 'BUF_QUANTUM' below current PRU read position. */
    if (!ts_data_ptr->catch_rx_tail) /* Tail should be catched */
    {
        /*
         * For example 
         * rx_head = 4 ---> 
         * rx_tail = 4 - 5 = -1 ---> 
         * rx_tail = 16 - 1 = 15
         * 15, 0, 1, 2, 3 (5)bufs to read
         */
        ts_data_ptr->rx_tail = pru_mcasp_dev_ptr->rx_head - BUF_QUANTUM;
        if ( ts_data_ptr->rx_tail < 0 ) 
            ts_data_ptr->rx_tail += BUFS_AMMNT;
        ts_data_ptr->catch_rx_tail = 1; /* Tail was catched */
        PRINTK_COND(DEBUG_RX_FLAG, MAG, "RX CATCH ts=%i h=%i t=%i\n", 
                    ts_idx, 
                    pru_mcasp_dev_ptr   ->rx_head, 
                    ts_data_ptr         ->rx_tail);
    }

    /*! Read. */
    while (byte_count >= BUF_SIZE_BYTES)
    {
        /* Sync abuffer before read. */
        page_addr = pru_mcasp_dev_ptr->ddr_start_addr \
                    + RX_BUF_BASE_OFFSET
                    + PAGE_SIZE * ts_idx * BUFS_AMMNT \
                    + PAGE_SIZE * ts_data_ptr->rx_tail;
        page_ptr = phys_to_page(page_addr);
        flush_dcache_page(page_ptr);
        /* Read from abuffer. */       
        memcpy (data, &pru_mcasp_dev_ptr->rx_bufs[ts_idx][ts_data_ptr->rx_tail].buf[0], BUF_SIZE_BYTES);
        /* Service pointers. */
        byte_count  -= BUF_SIZE_BYTES;  //reduce residue bytes count
        ret_val     += BUF_SIZE_BYTES;  //account read bytes
        data        += BUF_SIZE_BYTES;  //move pointer further  
        ts_data_ptr->rx_tail++;  //set next buffer
        if ( ts_data_ptr->rx_tail == BUFS_AMMNT ) ts_data_ptr->rx_tail = 0;
        if ( ts_data_ptr->rx_tail == pru_mcasp_dev_ptr->rx_head ) /* Stop read here. */
        {
            ts_data_ptr->can_read = 0;  // !!! BLOCK !!!
            PRINTK_COND(DEBUG_RX_FLAG, MAG, "RX RESET ts=%i h=t=%i\n", 
                        ts_idx, ts_data_ptr->rx_tail);
            goto out;
        }
    } //end while (byte_count
    
    /* This is some kind of awkwardness
     * to read data not aliquot to 'BUF_SIZE_BYTES'.
     * I saved it mostly for the debug purposes.
     */
    if (byte_count > 0)
    {
        /* Sync buffer before read. */
        page_addr = pru_mcasp_dev_ptr->ddr_start_addr \
                    + RX_BUF_BASE_OFFSET
                    + PAGE_SIZE * ts_idx * BUFS_AMMNT \
                    + PAGE_SIZE * ts_data_ptr->rx_tail;
        page_ptr = phys_to_page(page_addr);
        flush_dcache_page(page_ptr);
        /* Read from abuffer. */
        memcpy (data, &pru_mcasp_dev_ptr->rx_bufs[ts_idx][ts_data_ptr->rx_tail].buf[0], byte_count);
        /* Service pointers. */
        ts_data_ptr->rx_tail++; //set next buffer
        if ( ts_data_ptr->rx_tail == BUFS_AMMNT ) { ts_data_ptr->rx_tail = 0; }
        if ( ts_data_ptr->rx_tail == pru_mcasp_dev_ptr->rx_head ) 
        { 
            ts_data_ptr->can_read = 0; 
            PRINTK_COND(DEBUG_RX_FLAG, MAG, "RX RESET ts=%i h=t=%i\n", 
                        ts_idx, ts_data_ptr->rx_tail);
        } // !!! BLOCK !!!
        ret_val += byte_count;
    } //end if (byte_count > 0)
    PRINTK_COND(DEBUG_RX_FLAG, CYN, "RX ts=%i h=%i t=%i\n", 
            ts_idx, 
            pru_mcasp_dev_ptr   ->rx_head, 
            ts_data_ptr         ->rx_tail);
out:
    return ret_val;
    
} /* read_data_quantum */
/*--- End of the function ------------------------------------------------------------------------*/


/* ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);; */
ssize_t pru_mcasp_read  (
                        struct file *       filp,
                        char __user *       usr_buf,
                        size_t              count,
                        loff_t *            f_pos
                        )
{
    int                         ret_val             = 0;
    struct ts_data *            ts_data_ptr         = (struct ts_data *)            filp->private_data;
    struct pru_mcasp_device *   pru_mcasp_dev_ptr   = (struct pru_mcasp_device *)   ts_data_ptr->private;
    if (mutex_lock_interruptible(&ts_data_ptr->rd_mutex)) { return -ERESTARTSYS; } /* Sequential access to the read part. */
try_again:
        if (ts_data_ptr->can_read) //read is allowed
        {
            #if DYNAMIC_BUF
                /*! Allocate necessary buffer. */
                ts_data_ptr->rx_buf = (char *)kmalloc(count, GFP_KERNEL);
                if (!ts_data_ptr->rx_buf)
                {
                    mutex_unlock(&ts_data_ptr->rd_mutex);
                    return -ENOMEM;
                }
            #else
                if (count > sizeof(ts_data_ptr->rx_buf)) count = sizeof(ts_data_ptr->rx_buf);
            #endif
                ret_val = read_data_quantum     (
                                                pru_mcasp_dev_ptr,
                                                &ts_data_ptr->rx_buf[0],
                                                count,
                                                ts_data_ptr->ts_idx
                                                );
                
                *f_pos += ret_val;
            /*! Get data from user space. */
            if ( copy_to_user( usr_buf, &ts_data_ptr->rx_buf[0], ret_val ) )
            {
                __PRINTERR("Failed to put data to user.\n");
                ret_val = -EFAULT;
                goto out;
            } //end if ( copy_to_user
        }
        else //read is prohibited
        {
            if (filp->f_flags & O_NONBLOCK)
            {
                PRINTK_COND(DEBUG_R_BLOCK, MAG, "Nonblocking read : returning 'EAGAIN'.\n");
                ret_val = -EAGAIN;
                goto out;
            }
            else /* !!! BLOCK !!! */
            {
                PRINTK_COND(DEBUG_R_BLOCK, YEL, "Block.\n");
                ret_val = wait_event_interruptible_timeout  (
                                                            ts_data_ptr->read_q,
                                                            ts_data_ptr->can_read == 1,
                                                            READ_TIMEOUT_JFFS);
                if ( ret_val > 1 )
                {
                    PRINTK_COND(DEBUG_R_BLOCK, GRN, "Awaken.\n");
                    goto try_again;
                }
                if ( (ret_val == 0)||(ret_val == 1) )
                {
                    PRINTK_COND(DEBUG_R_BLOCK, RED, "Timeout.\n");
                    ret_val = -ETIME;
                    goto out;
                }
                if ( ret_val == -ERESTARTSYS )
                {
                    PRINTK_COND(DEBUG_R_BLOCK, RED, "Signal.\n");
                    goto out;
                } //end if ( ret_val
            } //end if (filp->f_flags
        } //end if (ts_data_ptr->can_write)
out:
    mutex_unlock(&ts_data_ptr->rd_mutex);
#if DYNAMIC_BUF
    kfree(ts_data_ptr->rx_buf);
#endif
    PRINTK_COND(DEBUG_R_BLOCK_SEVERE, CYN, "%i returned.\n", ret_val);
    return ret_val;
}
/*--- End of the function ------------------------------------------------------------------------*/

/*!
 * This function can accept only
 * (BUF_QUANTUM * BUF_SIZE_BYTES) bytes per call.
 * @return Real ammount of bytes written to the PRU-MCASP buffers.
 */
int write_data_quantum  (
                        struct pru_mcasp_device *   pru_mcasp_dev_ptr,  /* Provides data about buffers. */
                        void *                      data,               /* Data to copy. */
                        uint32_t                    byte_count,         /* How many data bytes to write. */
                        int                         ts_idx              /* Timeslot to use. */
                        )
{
    uint32_t            tx_head  = 0; ///< Index of the buf in time slot bufs array to START WRITING
    struct ts_data *    ts_data_ptr = &pru_mcasp_dev_ptr->ts_data[ts_idx];
    phys_addr_t         page_addr   = 0;
    struct page *       page_ptr    = NULL;
    int                 buf_count   = 0;
    int                 ret_val     = 0;

/*!
 * Idea is quite simple:
 * If process writes first time at all
 * or after a long timeout, the tail buffer pointer
 * should be used to define what buffer to use.
 */
    /*! Define position where start to write. 
     * It should be one buffer ahead of the current PRU position */
    if (!ts_data_ptr->catch_tx_tail) /* Tail should be catched */
    {
        tx_head = pru_mcasp_dev_ptr->tx_tail + 1;
        if ( tx_head == BUFS_AMMNT ) tx_head = 0;
        ts_data_ptr->catch_tx_tail = 1; /* Tail was catched */
    }
    else /* Tail have been catched already. */
    {
        tx_head = ts_data_ptr->tx_head; //restore TX head from previous write
    } //end if (!ts_data_ptr->catch_tx_tail)

    /*! Write. */
    while (byte_count >= BUF_SIZE_BYTES)
    {
        /* Write to abuffer. */
        memcpy (&pru_mcasp_dev_ptr->tx_bufs[ts_idx][tx_head].buf[0], data, BUF_SIZE_BYTES);
        /* Sync abuffer after write. */
        page_addr = pru_mcasp_dev_ptr->ddr_start_addr \
                    + PAGE_SIZE * ts_idx * BUFS_AMMNT \
                    + PAGE_SIZE * tx_head;
        page_ptr = phys_to_page(page_addr);
        flush_dcache_page(page_ptr);
        /* Service pointers. */
        byte_count  -= BUF_SIZE_BYTES;  //reduce count
        ret_val     += BUF_SIZE_BYTES;
        data        += BUF_SIZE_BYTES;  //move pointer further
        tx_head++;               //set next buffer
        if ( tx_head == BUFS_AMMNT ) { tx_head = 0; }
        buf_count++;                    //increment counter of written buffers
        if ( buf_count == BUF_QUANTUM ) /* Prohibit to write more than 'BUF_QUANTUM' data ahead. */
        {
            ts_data_ptr->can_write = 0; // !!! BLOCK !!!
            goto out;
        } //end if ( buf_count == BUF_QUANTUM )
    } //end while (byte_count

    /* This is some kind of awkwardness
     * to send data not aliquot to 'BUF_SIZE_BYTES'.
     * I saved it mostly for the debug purposes.
     */
    if (byte_count > 0)
    {
        /* Write to abufer. */
        memcpy (&pru_mcasp_dev_ptr->tx_bufs[ts_idx][tx_head].buf[0], data, byte_count);
        /* Sync buffer after write. */
        page_addr = pru_mcasp_dev_ptr->ddr_start_addr \
                    + PAGE_SIZE * ts_idx * BUFS_AMMNT \
                    + PAGE_SIZE * tx_head;
        page_ptr = phys_to_page(page_addr);
        flush_dcache_page(page_ptr);
        /* Service pointers. */
        tx_head++; //set next buffer
        if ( tx_head == BUFS_AMMNT ) { tx_head = 0; }
        buf_count++;
        if ( buf_count == BUF_QUANTUM ) { ts_data_ptr->can_write = 0; } // !!! BLOCK !!!
        ret_val += byte_count;
    } //end if (byte_count > 0)
out:
    ts_data_ptr->tx_head = tx_head; //remember TX head
    return ret_val;
}
/*--- End of the function ------------------------------------------------------------------------*/


/* ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *); */
ssize_t pru_mcasp_write (
                        struct file *       filp,
                        const char __user * usr_buf,
                        size_t              count,
                        loff_t *            f_pos
                        )
{
    int                         ret_val             = 0;
    struct ts_data *            ts_data_ptr         = (struct ts_data *)            filp->private_data;
    struct pru_mcasp_device *   pru_mcasp_dev_ptr   = (struct pru_mcasp_device *)   ts_data_ptr->private;
    if (mutex_lock_interruptible(&ts_data_ptr->wr_mutex)) { return -ERESTARTSYS; } /* Sequential access to the write part. */
try_again:
        if (ts_data_ptr->can_write) //write is allowed
        {
        #if DYNAMIC_BUF
            /*! Allocate necessary buffer. */
            ts_data_ptr->tx_buf = (char *)kmalloc(count, GFP_KERNEL);
            if (!ts_data_ptr->tx_buf)
            {
                mutex_unlock(&ts_data_ptr->wr_mutex);
                return -ENOMEM;
            }
        #else
            if (count > sizeof(ts_data_ptr->tx_buf)) count = sizeof(ts_data_ptr->tx_buf);
        #endif
            /*! Get data from user space. */
            if ( copy_from_user( &ts_data_ptr->tx_buf[0], usr_buf, count ) )
            {
                __PRINTERR("Failed to get data from user.\n");
                ret_val = -EFAULT;
                goto out;
            } //end if ( copy_from_user
            ret_val = write_data_quantum   (
                                            pru_mcasp_dev_ptr,
                                            &ts_data_ptr->tx_buf[0],
                                            count,
                                            ts_data_ptr->ts_idx
                                            );
            *f_pos += ret_val;
        }
        else //write is prohibited
        {
            if (filp->f_flags & O_NONBLOCK)
            {
                PRINTK_COND(DEBUG_W_BLOCK, MAG, "Nonblocking write : returning 'EAGAIN'.\n");
                ret_val = -EAGAIN;
                goto out;
            }
            else /* !!! BLOCK !!! */
            {
                PRINTK_COND(DEBUG_W_BLOCK, YEL, "Block.\n");
                ret_val = wait_event_interruptible_timeout  (
                                                            ts_data_ptr->write_q,
                                                            ts_data_ptr->can_write == 1,
                                                             WRITE_TIMEOUT_JFFS);
                if ( ret_val > 1 )
                {
                    PRINTK_COND(DEBUG_W_BLOCK, GRN, "Awaken.\n");
                    goto try_again;
                }
                if ( (ret_val == 0)||(ret_val == 1) )
                {
                    PRINTK_COND(DEBUG_W_BLOCK, RED, "Timeout.\n");
                    ret_val = -ETIME;
                    goto out;
                }
                if ( ret_val == -ERESTARTSYS )
                {
                    PRINTK_COND(DEBUG_W_BLOCK, RED, "Signal.\n");
                    goto out;
                } //end if ( ret_val
            } //end if (filp->f_flags
        } //end if (ts_data_ptr->can_write)
out:
    mutex_unlock(&ts_data_ptr->wr_mutex);
#if DYNAMIC_BUF
    kfree(ts_data_ptr->tx_buf);
#endif
    PRINTK_COND(DEBUG_W_BLOCK_SEVERE, CYN, "%i returned.\n", ret_val);
    return ret_val;
} //end pru_mcasp_write
/*--- End of the function ------------------------------------------------------------------------*/

int pru_mcasp_open (struct inode * inode_ptr, struct file * filp)
{
    struct ts_data * ts_data_ptr = container_of(inode_ptr->i_cdev, struct ts_data, cdev);
    filp->private_data = ts_data_ptr; /* For the other methods. */
    __PRINTK(CYN, "Device %s is opened.\n", &ts_data_ptr->dev_name[0]);
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

int pru_mcasp_release (struct inode * inode_ptr, struct file * filp)
{
    struct ts_data * ts_data_ptr = container_of(inode_ptr->i_cdev, struct ts_data, cdev);
    __PRINTK(CYN, "Device %s is released.\n", &ts_data_ptr->dev_name[0]);
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static const struct file_operations  pru_mcasp_dev_fops = {
    .owner      = THIS_MODULE,
    .read       = pru_mcasp_read,
    .write      = pru_mcasp_write,
    .open       = pru_mcasp_open,
    .release    = pru_mcasp_release,
    .poll       = pru_mcasp_poll, /* unsigned int (*poll) (struct file *, struct poll_table_struct *); */
};

#define PRU_MCASP_FIRST_MINOR   0
#define PRU_MCASP_DEVICES       TS_AMMNT //One 'cdev' for each RX - TX time slot couple.
int pru_mcasp_create_dev_iface(struct pru_mcasp_device * pru_mcasp_dev_ptr)
{

    int                 ret_val     = 0;
    int                 ts_idx      = 0;
    int                 ts_idx_tmp  = 0;
    struct ts_data *    ts_data_ptr = NULL;
    struct device *     tmp_dev     = 0; /* Just for error recognition. */
    __PRINTK(YEL, "Allocating driver numbers for the device '%s'.\n",
            &pru_mcasp_dev_ptr->rpdev->id.name[0]);
    ret_val = alloc_chrdev_region   (
                                    &pru_mcasp_dev_ptr->dev_num,                /* Output parameter for first assigned number. */
                                    PRU_MCASP_FIRST_MINOR,                      /* First of the requested range of minor numbers. */
                                    PRU_MCASP_DEVICES,                          /* The number of minor numbers required. */
                                    &pru_mcasp_dev_ptr->rpdev->id.name[0]       /* The name of the associated device or driver. */
                                    );
    if (ret_val < 0)
    {
        __PRINTERR( "Can't allocate device numbers for the device '%s'.\n",
                    pru_mcasp_dev_ptr->rpdev->id.name);
        return ret_val;
    } //end if (ret_val < 0)
    /* Creating separate '/dev' interface for each RX - TX time slot couple. */
    for ( ts_idx = 0; ts_idx < TS_AMMNT; ts_idx++ )
    {
        /* Prepare structure. */
        ts_data_ptr = &pru_mcasp_dev_ptr->ts_data[ts_idx]; //Allow future conversion to the separate routine.
        tmp_dev = NULL;
        ts_data_ptr->private = (void *)pru_mcasp_dev_ptr; //To extract back in the 'fops' callbacks.
        ts_data_ptr->ts_idx  = ts_idx;
        sprintf(&ts_data_ptr->dev_name[0], "%s_ts%i", &pru_mcasp_dev_ptr->rpdev->id.name[0], ts_idx);

        __PRINTK(YEL, "Initializing cdev mutexes for the device '%s'.\n",
                &ts_data_ptr->dev_name[0]);
        mutex_init(&ts_data_ptr->wr_mutex);
        mutex_init(&ts_data_ptr->rd_mutex);

        __PRINTK(YEL, "Initializing queues for the device '%s'.\n",
                &ts_data_ptr->dev_name[0]);
        init_waitqueue_head(&ts_data_ptr->read_q);
        init_waitqueue_head(&ts_data_ptr->write_q);

        __PRINTK(YEL, "Creating char device for the device '%s'.\n",
                &ts_data_ptr->dev_name[0]);
        /*
         * The same 'file_operations' callbacks set is used
         * for all the time slot char devices.
         * Recognition should be made in place.
         */
        cdev_init(&ts_data_ptr->cdev, &pru_mcasp_dev_fops);
        ts_data_ptr->cdev.owner = THIS_MODULE;
        ts_data_ptr->cdev.ops   = &pru_mcasp_dev_fops;
        ret_val = cdev_add  (
                            &ts_data_ptr->cdev,                     /* 'struct cdev *' */
                            pru_mcasp_dev_ptr->dev_num + ts_idx,    /* The first device number for which this device is responsible. */
                            1                                       /* The number of consecutive minor numbers corresponding to this device. */
                            );
        if (ret_val)
        {
            __PRINTERR( "Can't add 'cdev' for the device '%s'. ret_val = %d.\n",
                        &ts_data_ptr->dev_name[0], ret_val);
            goto unregister_chrdev;
        } //end if (ret_val < 0)
        __PRINTK(YEL, "Creating class for the device '%s'.\n",
                &ts_data_ptr->dev_name[0]);
        ts_data_ptr->class = class_create(THIS_MODULE, &ts_data_ptr->dev_name[0]);
        if ( IS_ERR(ts_data_ptr->class) )
        {
            ret_val = PTR_ERR(ts_data_ptr->class);
            __PRINTERR( "Can't create class for the device '%s'. ret_val = %d.\n",
                        &ts_data_ptr->dev_name[0], ret_val);
            goto delete_cdev;
        } //end if ( IS_ERR
        __PRINTK(MAG,"Creating device '%s'.\n", &ts_data_ptr->dev_name[0]);
            /* A "dev" file will be created, showing the dev_t for the device, if the dev_t is not 0,0. */
        tmp_dev = device_create (
                                ts_data_ptr->class,                     /* Pointer to the struct class that this device should be registered to. */
                                NULL,                                   /* Pointer to the parent struct device of this new device. */
                                pru_mcasp_dev_ptr->dev_num + ts_idx,    /* The 'dev_t' for the char device to be added. */
                                (void *)ts_data_ptr,                    /* The data to be added to the device for callbacks. */
                                &ts_data_ptr->dev_name[0]               /* String for the device's name. */
                                );
        if (IS_ERR(tmp_dev))
        {
            ret_val = PTR_ERR(tmp_dev);
            __PRINTERR( "Can't create '%s' device. ret_val = %d.\n",
                        &ts_data_ptr->dev_name[0], ret_val);
            goto destroy_class;
        } //end if ( IS_ERR
        __PRINTK(GRN, "Char device '/dev/%s' successfully initialized.\n"
                "%39s Major : %i. Minor : %i.\n",
                &ts_data_ptr->dev_name[0],
                "", MAJOR(pru_mcasp_dev_ptr->dev_num), MINOR(pru_mcasp_dev_ptr->dev_num));
        continue;
    destroy_class:
        ts_idx_tmp = ts_idx;
        while ( ts_idx_tmp >= 0 ){ class_destroy( pru_mcasp_dev_ptr->ts_data[ts_idx_tmp].class ); ts_idx_tmp--; }
    delete_cdev:
        ts_idx_tmp = ts_idx;
        while ( ts_idx_tmp >= 0 ){ cdev_del( &pru_mcasp_dev_ptr->ts_data[ts_idx_tmp].cdev );  ts_idx_tmp--; }
    unregister_chrdev:
        unregister_chrdev_region(pru_mcasp_dev_ptr->dev_num, PRU_MCASP_DEVICES);
        return -1;
    } //end for ( ts_idx
    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

int pru_mcasp_destroy_dev_iface(struct pru_mcasp_device * pru_mcasp_dev_ptr)
{
    int ts_idx = 0;
    struct ts_data * ts_data_ptr;
    for ( ts_idx = 0; ts_idx< TS_AMMNT; ts_idx++ )
    {
        __PRINTK(YEL, "Removing '/dev/' interface for the time slot #%i.\n", ts_idx);
        ts_data_ptr = &pru_mcasp_dev_ptr->ts_data[ts_idx]; //Allow future conversion to the separate routine.
        mutex_unlock(&ts_data_ptr->wr_mutex);
        mutex_unlock(&ts_data_ptr->rd_mutex);
        if (ts_data_ptr->class != NULL)
        {
            __PRINTK(YEL, "Removing '/dev/%s' interface.\n",
                    &ts_data_ptr->dev_name[0]);
            device_destroy(ts_data_ptr->class, pru_mcasp_dev_ptr->dev_num + ts_idx);
            class_destroy(ts_data_ptr->class);
        } //end if (ts_data_ptr->class
        cdev_del(&ts_data_ptr->cdev);
    } //end for ( ts_idx
    unregister_chrdev_region(pru_mcasp_dev_ptr->dev_num, PRU_MCASP_DEVICES);
    return 0;
}
/*--- End of function ----------------------------------------------------------------------------*/
