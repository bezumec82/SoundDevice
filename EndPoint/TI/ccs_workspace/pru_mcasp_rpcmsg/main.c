#include "main.h"
#include "resource_table_0.h"

/*!
 * @mainpage
 * This is the project to read-write McASP from PRU on TI Sitara devices.
 * PRU directly accesses register of dedicated McASP devices.
 */

/*!
 * @file
 * This file is containing main cycle of the program.
 */

volatile register uint32_t __R31;

#define PRU_CLK_SPEED           200 * 1000 * 1000
#define SAMPLE_RATE             16000
#define XDATA_TIMEOUT           (1 * PRU_CLK_SPEED / SAMPLE_RATE)

/*!
 * @name Direction flags
 * Used in RPC messages to ARM to say what direction is ready to give/take new data.
 * Used together with buffer index and timeslot index.
 * These three parameters describes what part of DDR was currently used by PRU
 * to read/write data to/from McASP.
 * @{
 */
#define DIRECTION_TX            0x1
#define DIRECTION_RX            0x2
/*! @} */

/*!
 * This structure represents data that is
 * exchanged between PRU and ARM.
 * @{
 */
struct rpc_msg {
    //out
    uint32_t    dir;            ///< Transmission direction.
    uint32_t    buf_idx;        ///< Buffer index.
    uint32_t    ts_idx;         ///< Timeslot index.
    //in
    uint32_t    tx_en;          ///< Transmission enabled flag
    uint32_t    rx_en;          ///< Reception enabled flag
    uint32_t    ddr_start_addr; ///< Physical address of start of audio buffers
}__packed__;
/*! @} */

/*! This structure is a package around real DDR buffer.
 * It will be used to form 2d array NUM_TX_SLOTS x NUM_TX_BUFS
 * around audio buffer. */
struct buffer {
    uint32_t *              buf; ///< Pointer to the start of the concrete buffer.
#if (LINKED_BUFS)
    struct buffer *         next;
#endif
};

struct buffer tx_bufs [NUM_TX_SLOTS] [NUM_TX_BUFS];
struct buffer rx_bufs [NUM_RX_SLOTS] [NUM_RX_BUFS];

#if RPC_MSG
    static struct rpc_msg msg =
    {
        .ddr_start_addr = 0xffffffff,
        .tx_en          = 0,
        .rx_en          = 0,
    };
    static uint16_t msg_len = sizeof(struct rpc_msg);
#else
    static struct rpc_msg msg =
    {
#if (BBB_AM335x)
        .ddr_start_addr = 0x80000000 + 0x20000000 - 0x01000000, /* 512 MiB - 16MiB */
#elif (IDK_AM5728)
        .ddr_start_addr = 0x80000000 + 0x40000000 - 0x01000000, /* 1GiB */
#endif
        .tx_en          = 1,
        .rx_en          = 1,
    };
#endif

uint32_t rx_bufs_ddr_start_addr;
uint32_t tx_bufs_ddr_start_addr;

int main(void)
{
    /* Create all necessary variables. */
#if (1)
    int16_t i;
    int16_t j;
    #if (DEBUG_BUF)
    int16_t k;
    #endif
    #if (MCASP_TX)
    uint32_t mcasp_TX_timeout_ctr  = 0;
    uint32_t tx_ts_idx     = 0; // Timeslot index
    uint32_t tx_buf_idx    = 0; // Index of the buf in time slot bufs array
    uint32_t tx_buf_cntr   = 0; // End point buf member counter
    #endif
    #if (MCASP_RX)
    uint32_t rx_ts_idx     = 0; // Timeslot index
    uint32_t rx_buf_idx    = 0; // Index of the buf in time slot bufs array
    uint32_t rx_buf_cntr   = 0; // End point buf member counter
    #endif
    #if (RPC_MSG)
    uint16_t                    src;
    uint16_t                    dst;
    struct pru_rpmsg_transport  transport;
    volatile uint8_t            *status;
    #endif
#endif
    /*! 1) Clear SYSCFG[STANDBY_INIT] to enable OCP master port. */
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

#if (RPC_MSG)
    /*! 2) Clear the status of the PRU-ICSS system event that the ARM will use to 'kick' us. */
    CT_INTC.SICR_bit.STATUS_CLR_INDEX = FROM_ARM_HOST;
    /*! 3) Make sure the Linux drivers are ready for RPC communication. */
    status = &resourceTable.rpmsg_vdev.status;
    while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));
    /*! 4) Initialize the RPMsg transport structure. */
    pru_rpmsg_init  (
                    &transport,
                    &resourceTable.rpmsg_vring0,
                    &resourceTable.rpmsg_vring1,
                    TO_ARM_HOST,
                    FROM_ARM_HOST
                    );
    /*! 4) Create the RPMsg channel between the PRU and ARM user space using the transport structure. */
    while   (
            pru_rpmsg_channel   (
                                RPMSG_NS_CREATE,
                                &transport,
                                CHAN_NAME,
                                CHAN_DESC,
                                CHAN_PORT
                                ) != PRU_RPMSG_SUCCESS
            )
        ;
receive_addr:
    /*! 5) Wait while ARM allocates memory
     * and sends to us starting address of this audio buffer. */
    while (1)
    {
        /*! 5.1) Check bit 30 of register R31 to see if the ARM has kicked us. */
        if (__R31 & HOST_INT)
        {
            /*! 5.2) Clear the event status. */
            CT_INTC.SICR_bit.STATUS_CLR_INDEX = FROM_ARM_HOST;
            /*! 5.3) Receive all available messages, multiple messages can be sent per kick. */
            if (
                    pru_rpmsg_receive   (
                                        &transport,
                                        &src,
                                        &dst,
                                        &msg,
                                        &msg_len
                                        ) != PRU_RPMSG_SUCCESS) goto start;
        } //end if (__R31 & HOST_INT)
    } //end while (1)

start:
    /*! If address of audio buffer doesn't received. Start again. */
    if (msg.ddr_start_addr == 0xffffffff) goto receive_addr;
#endif
    tx_bufs_ddr_start_addr = msg.ddr_start_addr;
    rx_bufs_ddr_start_addr = msg.ddr_start_addr + RX_BUF_BASE_OFFSET;


    /*! 6) Initialize McASP buffers system.
     * Booths sides (PRU and ARM) agree about how
     * many memory will be used for audio buffers.
     * There are two distinguish audio buffers - for TX and for RX.
     * Both buffers placed in memory subsequently. TX buf then RX buf.
     * Each of them consists of PAGE_SIZE x NUM_TX_BUFS and PAGE_SIZE x NUM_RX_BUFS accordingly.
     * For the quick access these buffers PRU uses 2d array of struct buffer. */
     /*! 6.1) Initialize McASP TX buffers system. */
#if MCASP_TX
    for (i = 0; i < NUM_TX_SLOTS; i++)
    {
        for (j = 0; j < NUM_TX_BUFS; j++)
        {
        #if (LINKED_BUFS)
            tx_bufs[i][j].next      = &tx_bufs[i][j+1];
        #endif
        #if (PAGE_ALIGNED_BUFS)
            tx_bufs[i][j].buf       = (uint32_t *)  ( tx_bufs_ddr_start_addr +
                                                    ( i * ( PAGE_SIZE * NUM_TX_BUFS ) ) +
                                                    ( j *   PAGE_SIZE ) );
        #else
            tx_bufs[i][j].buf       = (uint32_t *)  ( tx_bufs_ddr_start_addr +
                                                    ( i * (BUF_SIZE_BYTES * NUM_TX_BUFS ) ) +
                                                    ( j * BUF_SIZE_BYTES ) );
        #endif
        #if (DEBUG_BUF)
            for (k = 0; k < BUF_SIZE_WORDS; k++)
            {
                tx_bufs[i][j].buf[k] = (10000 * (i+1)) + (100 * (j+1)) + (k+1); //numbers to show on output
            }
        #endif
        } //end for(j
    #if (LINKED_BUFS)
        tx_bufs[i][j-1].next = &tx_bufs[i][0]; //Connect final to the first
    #endif
    } //end for (i
#endif
    /*-------------------------------------------*/
    /*! 6.2) Initialize McASP RX buffers system. */
    /*-------------------------------------------*/
#if MCASP_RX
    for (i = 0; i < NUM_RX_SLOTS; i++)
    {
        for (j = 0; j < NUM_RX_BUFS; j++)
        {
    #if (LINKED_BUFS)
            rx_bufs[i][j].next      = &rx_bufs[i][j+1];
    #endif
#if (PAGE_ALIGNED_BUFS)
            rx_bufs[i][j].buf       = (uint32_t *)  ( rx_bufs_ddr_start_addr +
                                                    ( i * ( PAGE_SIZE * NUM_RX_BUFS ) ) +
                                                    ( j *   PAGE_SIZE ) );
#else
            rx_bufs[i][j].buf       = (uint32_t *)  ( rx_bufs_ddr_start_addr +
                                                    ( i * (BUF_SIZE_BYTES * NUM_RX_BUFS ) ) +
                                                    ( j * BUF_SIZE_BYTES ) );
#endif
        } //end for(j
    #if (LINKED_BUFS)
        rx_bufs[i][j-1].next = &rx_bufs[i][0]; //Connect final to the first
    #endif
    } //end for (i
#endif

    /*----------------------------*/
    /*! 7) Hardware configuration */
    /*----------------------------*/
    #if (CLK_CFG)
        clk_cfg();
    #endif
    #if 	(DEBUG_LEDS_IDK_IDK_AM5728)
        setup_gpio_pads();
    #endif
    #if     (MCASP8)
        config_mcasp8_pads_idk_am5728();
    #elif   (MCASP1 & IDK_AM5728)
        config_mcasp1_pads_idk_am5728();
    #endif
    /* The control module includes status and control logic not addressed within the peripherals or the rest of the device infrastructure. */
    #if     (RUN_IN_ARM)
        #if     (MCASP0 & BBB_AM335x)
            config_mcasp0_pads_BBB();
        #elif   (MCASP1 & BBB_AM335x)
            config_mcasp1_pads_BBB();
        #endif
    #endif /* RUN_IN_ARM */
    init_mcasp();
    /*-------------------------*/
    /*! 8) MAIN PROGRAMM CYCLE */
    /*-------------------------*/
    while (1)
    {
    /*-----------------------*/
    /*! 8.1) McASP1 TX block */
    /*-----------------------*/
    #if MCASP_TX
        if (msg.tx_en)
        {
            if (MCASP_TXSTAT_bits_ptr->ERR) goto tx_error_handling;
            /*! 8.1.1) Read transmit data ready flag. If 1 -> put data in TX serializer buffer. */
            if (MCASP_TXSTAT_bits_ptr->DATA)
            {
                tx_ts_idx = MCASP_TXTDMSLOT_bits_ptr->SLOTCNT + 1; //Write data for the next slot
                if (tx_ts_idx == NUM_TX_SLOTS) //If new frame -> increment counter
                {
                    tx_ts_idx = 0;
                    tx_buf_cntr++;
                    if (tx_buf_cntr == BUF_SIZE_WORDS) //Set next buffer
                    {
                        tx_buf_idx++;
                        if (tx_buf_idx == NUM_TX_BUFS) { tx_buf_idx = 0; } //Circle
                        tx_buf_cntr = 0;
                    /*! 8.1.2) If new TX buffer in use -> send mailbox to ARM with new buf num. */
                    #if (RPC_MSG)
                        msg.buf_idx = tx_buf_idx;
                        msg.ts_idx  = tx_ts_idx; /* Not used actually. One message for two slots. */
                        msg.dir     = DIRECTION_TX;
                        pru_rpmsg_send  (
                                        &transport,
                                        dst,
                                        src,
                                        &msg,
                                        msg_len
                                        );
                    #endif
                    } //end if (tx_buf_cntr == BUF_SIZE_WORDS)
                }else if (tx_ts_idx > NUM_TX_SLOTS)
                    goto tx_error_handling;
        #if (DEBUG_RUNTIME_TX)
                tx_bufs[tx_ts_idx][tx_buf_idx].buf[tx_buf_cntr] = (10000 * (tx_ts_idx + 1)) + (100 * (tx_buf_idx + 1)) + tx_buf_cntr;
        #endif
                * MCASP_TXBUF_ptr = tx_bufs[tx_ts_idx][tx_buf_idx].buf[tx_buf_cntr];
        #if (!DEBUG_BUF)
                /*! 8.1.3) Zero buffer after data was sent. */
                tx_bufs[tx_ts_idx][tx_buf_idx].buf[tx_buf_cntr] = 0x0; //clear buffer after send
        #endif
                mcasp_TX_timeout_ctr = 0; //Clear timeout counter
            }
            else //if (MCASP_TXSTAT_bits_ptr->DATA)
            {
                mcasp_TX_timeout_ctr++;
                if (mcasp_TX_timeout_ctr == XDATA_TIMEOUT) goto tx_error_handling;
            } //end if (MCASP_TXSTAT_bits_ptr->DATA)
        } //end if (msg.tx_en)
    #endif /* MCASP_TX */

    /*-----------------------*/
    /*! 8.2) McASP1 RX block */
    /*-----------------------*/
    #if MCASP_RX
        if (msg.rx_en)
        {
            if (MCASP_RXSTAT_bits_ptr->ERR) goto rx_error_handling;
            /*! 8.2.1) Read receive data ready flag. If 1 -> get data from RX serializer buffer. */
            if (MCASP_RXSTAT_bits_ptr->DATA)
            {
                //This is actual only for <32 timeslot TDM
                if (MCASP_RXTDMSLOT_bits_ptr->SLOTCNT) //not slot 0 currently in reception
                    rx_ts_idx = MCASP_RXTDMSLOT_bits_ptr->SLOTCNT - 1; //Next slot currently in arrival
                else //slot 0 currently in reception
                    rx_ts_idx = NUM_RX_SLOTS - 1;
                if (!rx_ts_idx)
                {
                    rx_buf_cntr++;
                    if (rx_buf_cntr == BUF_SIZE_WORDS) //Flip buffers
                    {
                        rx_buf_idx++;
                        if (rx_buf_idx == NUM_RX_BUFS) rx_buf_idx = 0;
                        rx_buf_cntr = 0;
                    /*! 8.2.2) If new RX buffer in use -> send mailbox to ARM with new buf num. */
                    #if (RPC_MSG)
                        msg.buf_idx = rx_buf_idx;
                        msg.ts_idx  = rx_ts_idx;
                        msg.dir     = DIRECTION_RX;
                        pru_rpmsg_send  (
                                        &transport,
                                        dst,
                                        src,
                                        &msg,
                                        msg_len
                                        );
                    #endif
                    } //end if (rx_buf_cntr == BUF_SIZE_WORDS)
                } //end if (!rx_ts_idx)
                rx_bufs[rx_ts_idx][rx_buf_idx].buf[rx_buf_cntr] = * MCASP_RXBUF_ptr;
            } //end if (MCASP_RXSTAT_bits_ptr->DATA)
        } //end if (msg.rx_en)
    #endif /* MCASP_RX */

    #if (RPC_MSG)
        if (__R31 & HOST_INT)
        {
            /* Clear the event status */
            CT_INTC.SICR_bit.STATUS_CLR_INDEX = FROM_ARM_HOST;
            pru_rpmsg_receive   (
                                &transport,
                                &src,
                                &dst,
                                &msg,
                                &msg_len
                                );
            if ((!msg.rx_en)&&(!msg.tx_en))
            {
            reset_mcasp:
                * MCASP_GBLCTL_ptr = 0x0;
                uint32_t timeout_counter = 0;
                while (* MCASP_GBLCTL_ptr)
                {
                    timeout_counter++;
                    if (timeout_counter>INIT_TIMEOUT_CNT)
                    {
                        goto reset_mcasp;
                    }
                }//end while (* MCASP_GBLCTL_ptr)
                goto halt;
            }//end if ((!msg.rx_en)&&(!msg.tx_en))
        }//end if (__R31 & HOST_INT)
    #endif
        continue;

    #if DEBUG_MCASP_ERRS
        uint32_t err = 0;
    #endif

    #if (MCASP_TX)
        tx_error_handling:
        #if DEBUG_MCASP_ERRS
            err = MCASP8_TXSTAT_bits_ptr->ERR;
        #endif
        #if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_industial_2_red();
        #endif
            init_mcasp();
            continue;
    #endif /* MCASP_TX */

    #if (MCASP_RX)
        rx_error_handling:
        #if DEBUG_MCASP_ERRS
            err = MCASP8_TXSTAT_bits_ptr->ERR;
        #endif
        #if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_industial_3_red();
        #endif
            init_mcasp();
            continue;
    #endif /* MCASP_RX */
    } //end while(1)
#if (RPC_MSG)
    halt:
    while (1)
        ;
#endif
}
