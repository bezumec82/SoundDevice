#include "mcasp_rpmsg_mod.h"

#if (DEBUG_RPC)
    static uint32_t counter = 0;
#endif

/*!
 * Message from PRU.
 * PRU says what direction and
 * index of the buffer in use,
 * each time it switches buffers.
 * In present configuration:
 * 1B (buffer) = 64w (32 bit words)
 * message rate :
 * 64w/16000kHz * 1000ms/s = 4ms/B.
 */
struct rpc_msg {
    //in
    uint32_t    dir; //transmission direction
    uint32_t    buf_idx;
    uint32_t    ts_idx; /* Not used currently */
    //out
    uint32_t    tx_en;
    uint32_t    rx_en;
    uint32_t    ddr_start_addr;
}__packed__;
static struct rpc_msg msg;

static void gather_sysfs(int ts_idx, uint32_t dir, struct pru_mcasp_device * pru_mcasp_dev_ptr)
{
    struct statistics * stat_ptr = &pru_mcasp_dev_ptr->stat;
    if (ts_idx == 0) {
        if (dir == DIRECTION_TX) {
            if(!stat_ptr->cntr_tx_msg_ts1) { getnstimeofday(&stat_ptr->frst_tx_msg_ts1); }  /* Fix time of the first msg. reception. */
                                             getnstimeofday(&stat_ptr->last_tx_msg_ts1);    /* Fix time of last received message. */
                                                             stat_ptr->cntr_tx_msg_ts1++; } /* Increment message counter. */
        if (dir == DIRECTION_RX) {
            if(!stat_ptr->cntr_rx_msg_ts1) { getnstimeofday(&stat_ptr->frst_rx_msg_ts1); }  /* Fix time of the first msg. reception. */
                                             getnstimeofday(&stat_ptr->last_rx_msg_ts1);    /* Fix time of last received message. */
                                                             stat_ptr->cntr_rx_msg_ts1++; } /* Increment message counter. */
    } //end if (ts_idx == 0)
    if(ts_idx == 1) {
        if (dir == DIRECTION_TX) {
            if(!stat_ptr->cntr_tx_msg_ts2) { getnstimeofday(&stat_ptr->frst_tx_msg_ts2); }  /* Fix time of the first msg. reception. */
                                             getnstimeofday(&stat_ptr->last_tx_msg_ts2);    /* Fix time of last received message. */
                                                             stat_ptr->cntr_tx_msg_ts2++; } /* Increment message counter. */
        if (dir == DIRECTION_RX) {
            if(!stat_ptr->cntr_rx_msg_ts2) { getnstimeofday(&stat_ptr->frst_rx_msg_ts2); }  /* Fix time of the first msg. reception. */
                                             getnstimeofday(&stat_ptr->last_rx_msg_ts2);    /* Fix time of last received message. */
                                                             stat_ptr->cntr_rx_msg_ts2++; } /* Increment message counter. */
    } //end if(ts_idx == 1)
}

static int rpmsg_pru_mcasp_cb   (
                                struct rpmsg_device *   rpdev,
                                void *                  data,
                                int                     len,
                                void *                  priv,
                                u32                     src
                                )
{
    #if (DEBUG_RPC)
        struct timespec         time                = 0;
        u32                     curr_time           = 0;
    #endif
    int                         ts_idx              = 0;
    int                         tail                = 0;
    int                         head                = 0;
    #if (DEBUG_BUF_RUNTIME)
        int                     buf_cntr            = 0;
        uint32_t                tx_buf_idx          = 0; ///< Index of the buf in time slot bufs array to start writing
        struct page *           page_ptr            = NULL;
        phys_addr_t             page_addr           = 0;
    #endif
    struct pru_mcasp_device *   pru_mcasp_dev_ptr   = dev_get_drvdata(&rpdev->dev);
    struct rpc_msg *            msg_ptr             = (struct rpc_msg *)data;
    struct ts_data *            ts_data_ptr         = NULL;

    if (msg_ptr->dir == DIRECTION_TX)
    {
        pru_mcasp_dev_ptr->tx_tail = msg_ptr->buf_idx; /* Save data about buffer PRU-MCASP position. */
        for (ts_idx = 0; ts_idx < TS_AMMNT; ts_idx++) /* Move through timeslots. */
        {
            gather_sysfs(ts_idx, DIRECTION_TX, pru_mcasp_dev_ptr);
            ts_data_ptr = &pru_mcasp_dev_ptr->ts_data[ts_idx];
            head        = ts_data_ptr       ->tx_head;
            tail        = pru_mcasp_dev_ptr ->tx_tail;
            if (ts_data_ptr->catch_tx_tail) /* Flag is set. Write process wasn't interrupted. */
            {
                if  ( /* PRU reaching ARM --> write space availible. Tail(PRU) 'TX_BUF_GAP' bufs back from head(ARM). */
                        ( (head/*ARM*/ - tail/*PRU*/) == TX_BUF_GAP )
                        ||
                        ( (head/*ARM*/ - tail/*PRU*/) == (TX_BUF_GAP - BUFS_AMMNT) )
                    )
                {
                    ts_data_ptr->can_write = 1; /* Allow write operation. */
                    wake_up_interruptible(&ts_data_ptr->write_q);
                    PRINTK_COND(DEBUG_TX_FLAG, GRN, "TX SET ts=%i h=%i t=%i\n", ts_idx, head, tail);
                }
                if ( head == tail ) /* Tail(PRU) achieved head(ARM). */
                {
                    ts_data_ptr->catch_tx_tail = 0; /* Reset catch flag. Tail should be catched again. */
                    //printk(RED"TX GAP ts=%i h=t=%i;\n"NORM, ts_idx, head);
                }
            } else { ts_data_ptr->can_write = 1; } /* Allow write operation. */
        #if (DEBUG_BUF_RUNTIME) //writing data forward
            tx_buf_idx = msg_ptr->buf_idx + 1;
            if (tx_buf_idx >= BUFS_AMMNT) { tx_buf_idx -= BUFS_AMMNT; }
            for (ts_idx = 0; ts_idx < TS_AMMNT; ts_idx++) /* Move through timeslots. */
            {
                for (buf_cntr = 0; buf_cntr < BUF_SIZE_WORDS; buf_cntr++)
                {
                    pru_mcasp_dev_ptr->tx_bufs[ts_idx][tx_buf_idx].buf[buf_cntr] \
                                                        = ( 10000 * ( ts_idx        + 1 ) ) \
                                                        + ( 100 *   ( tx_buf_idx    + 1 ) ) \
                                                        +           ( buf_cntr      + 1 ); //numbers to show at the output
                } //end for (buf_cntr
                page_addr = pru_mcasp_dev_ptr->ddr_start_addr \
                            + PAGE_SIZE * ts_idx * BUFS_AMMNT \
                            + PAGE_SIZE * tx_buf_idx;
                page_ptr = phys_to_page(page_addr);
                flush_dcache_page(page_ptr);
            } //end for (i
            #if (OVERKILL_FLUSH)
                flush_cache_all();
            #endif /* OVERKILL_FLUSH */
        #endif /* DEBUG_BUF_RUNTIME */
        } //end for (ts_idx
    } //end if (msg_ptr->dir == DIRECTION_TX)

    if (msg_ptr->dir == DIRECTION_RX)
    {
        pru_mcasp_dev_ptr->rx_head = msg_ptr->buf_idx; /* Save data about buffer PRU-MCASP position. */
        for (ts_idx = 0; ts_idx < TS_AMMNT; ts_idx++) /* Move through timeslots. */
        {
            gather_sysfs(ts_idx, DIRECTION_RX, pru_mcasp_dev_ptr);
            ts_data_ptr = &pru_mcasp_dev_ptr->ts_data[ts_idx];
            head        = pru_mcasp_dev_ptr ->rx_head;
            tail        = ts_data_ptr       ->rx_tail;
            if (ts_data_ptr->catch_rx_tail) /* Flag is set. Read process wasn't interrupted. */
            {
                if  ( /* ARM fall back from PRU. --> read data availible. */
                        ( (head/*PRU*/ - tail/*ARM*/) == RX_BUF_GAP )
                        ||
                        ( (head/*PRU*/ - tail/*ARM*/) == (RX_BUF_GAP - BUFS_AMMNT) )
                    ) /* Tail(ARM) 'RX_BUF_GAP' bufs back from head(PRU). */
                {
                    ts_data_ptr->can_read = 1;  /* Allow read operation. */
                    wake_up_interruptible(&ts_data_ptr->read_q);
                    PRINTK_COND(DEBUG_RX_FLAG, GRN, "RX SET ts=%i h=%i t=%i\n", ts_idx, head, tail);
                }
                if ( head == tail ) /* Head(PRU) run full bufs cycle and achieved tail(ARM). */
                {
                    ts_data_ptr->catch_rx_tail = 0; /* Reset catch flag. Tail should be catched again. */
                    printk(RED"RX GAP ts=%i h=t=%i\n"NORM, ts_idx, head);
                }
            } else { ts_data_ptr->can_read = 1; }; /* Allow read operation. */
        } //end for (ts_idx
    } //end if (msg_ptr->dir == DIRECTION_RX)
    #if (DEBUG_RPC)
        #if     (0)
            __PRINTK(CYN, "dir %-2d, ts #%-2d, buf #%-2d (src: 0x%x)\n",
            msg_ptr->dir, msg_ptr->ts_idx, msg_ptr->buf_idx, src);
        #elif   (1)
            if ( msg_ptr->dir == DIRECTION_TX )
            {
                if ( msg_ptr->buf_idx%8 == 0 ) //16 bufs in cycle, twice a flip
                {
                    time        = current_kernel_time();
                    curr_time   = (u32)(time.tv_nsec/1000000);
                    printk("%-2d|%-3ums\n", msg_ptr->buf_idx, curr_time);
                }
            }
        #elif   (0)
            if ( msg_ptr->dir == DIRECTION_TX )
            {
                counter++;
                if ( counter%4 == 0 ) //each 4 msgs
                {
                    time        = current_kernel_time();
                    curr_time   = (u32)(time.tv_nsec/1000000);
                    printk("%-2d|%-3ums\n", msg_ptr->buf_idx, curr_time);
                }
            }
        #endif
    #endif /* DEBUG_RPC */
        return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static int rpmsg_pru_mcasp_probe ( struct rpmsg_device * rpdev )
{
    int ret;
    int i;
    int j;
    #if (DEBUG_BUF_FILL)
        int k;
    #endif
    struct pru_mcasp_device *   pru_mcasp_dev_ptr = NULL;
    uint32_t tx_bufs_base_addr;
    uint32_t rx_bufs_base_addr;

    __PRINTK(GRN, "New channel: 0x%x -> 0x%x. Device : %s.\n",
            rpdev->src, rpdev->dst, rpdev->id.name);

    pru_mcasp_dev_ptr = devm_kzalloc(&rpdev->dev, sizeof(struct pru_mcasp_device), GFP_KERNEL);
    if (!pru_mcasp_dev_ptr) return -ENOMEM;
    pru_mcasp_dev_ptr->rpdev = rpdev;    

    /* Allocate audio buffers space. */
    pru_mcasp_dev_ptr->abuf = (void *)__get_free_pages(GFP_KERNEL, get_order(ABUFS_SIZE));
    if (!pru_mcasp_dev_ptr->abuf) return -ENOMEM;
    pru_mcasp_dev_ptr->ddr_start_addr = virt_to_phys(pru_mcasp_dev_ptr->abuf);
    __PRINTK(GRN, "Audio buffers successfully alloccated.\n"
            "%39s DDR address for audio bufs : 0x%x.\n",
            "", (unsigned int)pru_mcasp_dev_ptr->ddr_start_addr);
    /*----------------------------------*/
    /*! Initialize McASP buffers system */
    /*----------------------------------*/
    tx_bufs_base_addr = (uint32_t)pru_mcasp_dev_ptr->abuf;
    rx_bufs_base_addr = tx_bufs_base_addr + RX_BUF_BASE_OFFSET;
    for (i = 0; i < TS_AMMNT; i++)
    {
        for (j = 0; j < BUFS_AMMNT; j++)
        {
        #if (LINKED_BUFS)
            pru_mcasp_dev_ptr->tx_bufs[i][j].next   = &pru_mcasp_dev_ptr->tx_bufs[i][j+1];
            pru_mcasp_dev_ptr->rx_bufs[i][j].next   = &pru_mcasp_dev_ptr->rx_bufs[i][j+1];
        #endif
        #if (PAGE_ALIGNED_BUFS)
            pru_mcasp_dev_ptr->tx_bufs[i][j].buf    = (uint32_t *)  ( tx_bufs_base_addr + \
                                                                    ( i * ( PAGE_SIZE * BUFS_AMMNT ) ) + \
                                                                    ( j *   PAGE_SIZE ) );
            pru_mcasp_dev_ptr->rx_bufs[i][j].buf    = (uint32_t *)  ( rx_bufs_base_addr + \
                                                                    ( i * ( PAGE_SIZE * BUFS_AMMNT ) ) + \
                                                                    ( j *   PAGE_SIZE ) );
        #else
            pru_mcasp_dev_ptr->tx_bufs[i][j].buf    = (uint32_t *)  ( tx_bufs_base_addr + \
                                                                    ( i * (BUF_SIZE_BYTES * BUFS_AMMNT ) ) + \
                                                                    ( j *  BUF_SIZE_BYTES ) );
            pru_mcasp_dev_ptr->rx_bufs[i][j].buf    = (uint32_t *)  ( rx_bufs_base_addr + \
                                                                    ( i * (BUF_SIZE_BYTES * BUFS_AMMNT ) ) + \
                                                                    ( j *  BUF_SIZE_BYTES ) );
        #endif /* PAGE_ALIGNED_BUFS */
        #if (DEBUG_BUF_FILL)
            for (k = 0; k < BUF_SIZE_WORDS; k++)
            {
                pru_mcasp_dev_ptr->tx_bufs[i][j].buf[k] = (10000 * (i+1)) + (100 * (j+1)) + (k+1); //numbers to show at the output
            } //end for (k
        #endif
        } //end for(j
    #if (LINKED_BUFS)
        pru_mcasp_dev_ptr->tx_bufs[i][j-1].next = &pru_mcasp_dev_ptr->tx_bufs[i][0]; //Connect final to the first
        pru_mcasp_dev_ptr->rx_bufs[i][j-1].next = &pru_mcasp_dev_ptr->rx_bufs[i][0]; //Connect final to the first
    #endif
    } //end for (i
    flush_cache_all();

    if (pru_mcasp_create_dev_iface(pru_mcasp_dev_ptr) != 0)
    {
        __PRINTERR("Can't create dev interface.\n");
        return -1;
    }
    dev_set_drvdata(&rpdev->dev, pru_mcasp_dev_ptr);
    /* If everything above was successfull, allow PRU to make its work */
    msg.ddr_start_addr = (uint32_t)pru_mcasp_dev_ptr->ddr_start_addr;
    msg.tx_en = 1; //allow TX
    msg.rx_en = 1; //allow RX
    /* Send a message to the PRU */
    ret = rpmsg_send    (
                        rpdev->ept,
                        (void *)&msg,
                        sizeof(struct rpc_msg)
                        );
    if (ret)
    {
        __PRINTERR("Send failed. %d returned.\n", ret);
        return ret;
    }
    else __PRINTK(GRN, "RPC message sent.\n");
    
    ret = sysfs_create_group(&rpdev->dev.kobj, &pru_mcasp_group);
    if (ret) { __PRINTERR("'sysfs' group creation failed.\n"); }
    else     { __PRINTK(GRN, "'sysfs' group created.\n"); }

    return 0;
}
/*--- End of the function ------------------------------------------------------------------------*/

static void rpmsg_pru_mcasp_remove(struct rpmsg_device * rpdev)
{
    int     ret_val;
    struct pru_mcasp_device * pru_mcasp_dev_ptr = dev_get_drvdata(&rpdev->dev);
    msg.tx_en = 0; //Stop mcasp TX at PRU side
    msg.rx_en = 0; //Stop mcasp RX at PRU side
    ret_val = rpmsg_send(
                        rpdev->ept,
                        (void *)&msg,
                        sizeof(struct rpc_msg)
                        );
    if (ret_val)
    {
        __PRINTERR("Send failed. %d returned. PRU not stopped!!!\n", ret_val);
        return;
    }
    mdelay(1000);
    pru_mcasp_destroy_dev_iface(pru_mcasp_dev_ptr);
    free_pages((unsigned long)pru_mcasp_dev_ptr->abuf, get_order(ABUFS_SIZE));
    sysfs_remove_group(&rpdev->dev.kobj, &pru_mcasp_group);
    __PRINTK(YEL, "RPC MSG PRU-MCASP client driver is removed.\n");
}
/*--- End of the function ------------------------------------------------------------------------*/

static struct rpmsg_device_id rpmsg_pru_mcasp_id_table[] =
{
        /* RPMSG_NAME_SIZE = 32 */
        { .name = "mcasp0_pru0" },
        { .name = "mcasp0_pru1" },
        
        { .name = "mcasp1_pru0" }, 
        { .name = "mcasp1_pru1" },
        
        { .name = "mcasp8_pru0" },
        { .name = "mcasp8_pru1" },
        { /* Sentinel */ },
};
MODULE_DEVICE_TABLE(rpmsg, rpmsg_pru_mcasp_id_table);

static struct rpmsg_driver rpmsg_pru_mcasp_client =
{
        .drv.name       = KBUILD_MODNAME,
        .id_table       = rpmsg_pru_mcasp_id_table,
        .probe          = rpmsg_pru_mcasp_probe,
        .remove         = rpmsg_pru_mcasp_remove,
        .callback       = rpmsg_pru_mcasp_cb,
};
module_rpmsg_driver(rpmsg_pru_mcasp_client);

MODULE_DESCRIPTION("PRU MCASP RPC driver");
MODULE_LICENSE("GPL v2");
