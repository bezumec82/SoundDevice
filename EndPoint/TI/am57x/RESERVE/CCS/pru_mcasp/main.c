#include "main.h"

#define PRU_CLK_SPEED           200 * 1000 * 1000
#define SAMPLE_RATE             16000
#define XDATA_TIMEOUT           (1 * PRU_CLK_SPEED / SAMPLE_RATE)

/*! This structure defines one single buffer. */
struct buffer {
#if BUF_DEBUG
    uint32_t buf_num;
    uint32_t ts_num;
#endif
    uint32_t *      buf;
    struct buffer * next;
};

#if (MCASP_ABSTRACTION || BUFS_ABSTRACTION)
    struct mcasp  {
        uint16_t                num_slots;
        uint16_t                num_bufs_per_slot;
        uint32_t                bufs_ddr_addr_start;
        uint32_t                buf_size_bytes;
        uint32_t                buf_size_words;
        struct buffer *         bufs;
        #if (MCASP_ABSTRACTION)
            uint32_t                ts_idx;
            uint32_t                buf_idx;
            uint32_t                buf_cntr;

            struct MCASP_STAT *     MCASP_STAT_bits_ptr;
            struct MCASP_TDMSLOT *  MCASP_TDMSLOT_bits_ptr;
            uint32_t *              MCASP_BUF_ptr;
            void (*init_mcasp) (void);
            uint32_t                timeout_cntr;
        #endif
    };
#endif

#if (MCASP8_TX)
struct buffer tx8_bufs [NUM_TX8_SLOTS] [NUM_TX8_BUFS];
    #if (MCASP_ABSTRACTION || BUFS_ABSTRACTION)
    struct mcasp mcasp8_tx = {
        .num_slots                  = NUM_TX8_SLOTS,
        .num_bufs_per_slot          = NUM_TX8_BUFS,
        .bufs_ddr_addr_start        = MCASP8_TX_BUFS_START_ADDR,
        .buf_size_bytes             = BUF_SIZE_BYTES,
        .buf_size_words             = BUF_SIZE_WORDS,
        .bufs                       = (struct buffer *) tx8_bufs,
        #if (MCASP_ABSTRACTION)
            .MCASP_STAT_bits_ptr        = (struct MCASP_STAT *)     (MCASP8_CFG_ADDR + MCASP_TXSTAT_OFF),
            .MCASP_TDMSLOT_bits_ptr     = (struct MCASP_TDMSLOT *)  (MCASP8_CFG_ADDR + MCASP_TXTDMSLOT_OFF),
            .MCASP_BUF_ptr              = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_TXBUF_00_OFF),
            .init_mcasp                 = init_mcasp8,
            .timeout_cntr               = 0,
        #endif
    };
    #endif
#endif

#if (MCASP8_RX)
struct buffer rx8_bufs [NUM_RX8_SLOTS] [NUM_RX8_BUFS];
    #if (MCASP_ABSTRACTION || BUFS_ABSTRACTION)
    struct mcasp mcasp8_rx = {
        .num_slots                  = NUM_RX8_SLOTS,
        .num_bufs_per_slot          = NUM_RX8_BUFS,
        .bufs_ddr_addr_start        = MCASP8_RX_BUFS_START_ADDR,
        .buf_size_bytes             = BUF_SIZE_BYTES,
        .bufs                       = (struct buffer *) rx8_bufs,
        #if (MCASP_ABSTRACTION)
            .MCASP_STAT_bits_ptr        = (struct MCASP_STAT *)     (MCASP8_CFG_ADDR + MCASP_RXSTAT_OFF),
            .MCASP_TDMSLOT_bits_ptr     = (struct MCASP_TDMSLOT *)  (MCASP8_CFG_ADDR + MCASP_RXTDMSLOT_OFF),
            .MCASP_BUF_ptr              = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_RXBUF_01_OFF),
            .init_mcasp                 = init_mcasp8,
            .timeout_cntr               = 0,
        #endif
    };
    #endif
#endif

#if (MCASP1_TX)
struct buffer tx1_bufs [NUM_TX1_SLOTS] [NUM_TX1_BUFS];
    #if (MCASP_ABSTRACTION || BUFS_ABSTRACTION)
    struct mcasp mcasp1_tx = {
        .num_slots                  = NUM_TX1_SLOTS,
        .num_bufs_per_slot          = NUM_TX1_BUFS,
        .bufs_ddr_addr_start        = MCASP1_TX_BUFS_START_ADDR,
        .buf_size_bytes             = BUF_SIZE_BYTES,
        .buf_size_words             = BUF_SIZE_WORDS,
        .bufs                       = (struct buffer *) tx1_bufs,
        #if (MCASP_ABSTRACTION
            .MCASP_STAT_bits_ptr        = (struct MCASP_STAT *)     (MCASP1_CFG_ADDR + MCASP_TXSTAT_OFF),
            .MCASP_TDMSLOT_bits_ptr     = (struct MCASP_TDMSLOT *)  (MCASP1_CFG_ADDR + MCASP_TXTDMSLOT_OFF),
            .MCASP_BUF_ptr              = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_TXBUF_08_OFF),
            .init_mcasp                 = init_mcasp1,
            .timeout_cntr               = 0,
        #endif
    };
    #endif
#endif

#if (MCASP1_RX)
struct buffer rx1_bufs [NUM_RX1_SLOTS] [NUM_RX1_BUFS];
#if (MCASP_ABSTRACTION || BUFS_ABSTRACTION)
    struct mcasp mcasp1_rx = {
        .num_slots                  = NUM_RX1_SLOTS,
        .num_bufs_per_slot          = NUM_RX1_BUFS,
        .bufs_ddr_addr_start        = MCASP1_RX_BUFS_START_ADDR,
        .buf_size_bytes             = BUF_SIZE_BYTES,
        .bufs                       = (struct buffer *) rx1_bufs,
        #if (MCASP_ABSTRACTION
            .MCASP_STAT_bits_ptr        = (struct MCASP_STAT *)     (MCASP1_CFG_ADDR + MCASP_RXSTAT_OFF),
            .MCASP_TDMSLOT_bits_ptr     = (struct MCASP_TDMSLOT *)  (MCASP1_CFG_ADDR + MCASP_RXTDMSLOT_OFF),
            .MCASP_BUF_ptr              = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_RXBUF_14_OFF),
            .init_mcasp                 = init_mcasp1,
            .timeout_cntr               = 0,
        #endif
    };
    #endif
#endif

#if BUFS_ABSTRACTION
static inline void init_bufs ( struct mcasp * mcasp_ptr )
{
    int i;
    int j;
    int k;
    /*-----------------------*/
    /* Initialize TX buffers */
    /*-----------------------*/
    struct buffer * bufs;
    struct buffer * buf_2d_arr = mcasp_ptr->bufs;
    for (i = 0; i < mcasp_ptr->num_slots; i++)
    {
        bufs  = (struct buffer *) ( (uint32_t)(buf_2d_arr) + (i * sizeof(struct buffer) * mcasp_ptr->num_bufs_per_slot) );
        for (j = 0; j < mcasp_ptr->num_bufs_per_slot - 1; j++)
        {
            bufs->next          = (struct buffer *) ( (uint32_t)(bufs) + sizeof(struct buffer) );
            bufs->buf           = (uint32_t *)      ( mcasp_ptr->bufs_ddr_addr_start +
                                                    ( i * mcasp_ptr->buf_size_bytes * mcasp_ptr->num_bufs_per_slot ) +
                                                    ( j * mcasp_ptr->buf_size_bytes)
                                                    );
    #if BUF_DEBUG
            bufs->buf_num       = j + 1;
            bufs->ts_num        = i + 1;
            for (k = 0; k < mcasp_ptr->buf_size_words; k++){
                bufs->buf[k] = (10000 * (bufs->ts_num)) + (100 * (bufs->buf_num)) + k; //numbers to show on output
            }
    #endif
            bufs = (struct buffer *) ( (uint32_t)(bufs) + sizeof(struct buffer) );
        } //end for(j
        bufs->next = (struct buffer *) ( (uint32_t)(buf_2d_arr) + (i * sizeof(struct buffer) * mcasp_ptr->num_bufs_per_slot) ); //Connect final to the first
    #if BUF_DEBUG
        bufs->buf_num       = j + 1;
        bufs->ts_num        = i + 1;
        for (k = 0; k < mcasp_ptr->buf_size_words; k++){
            bufs->buf[k] = (10000 * (bufs->ts_num)) + (100 * (bufs->buf_num)) + k; //numbers to show on output
        }
    #endif
    } //end for (i
}
#endif

#if MCASP_ABSTRACTION
static inline void mcasp_tx ( struct mcasp * mcasp_ptr )
{
    if (mcasp_ptr->MCASP_STAT_bits_ptr->ERR) goto tx_error_handling;
    if (mcasp_ptr->MCASP_STAT_bits_ptr->DATA)
    {
        mcasp_ptr->ts_idx = mcasp_ptr->MCASP_TDMSLOT_bits_ptr->SLOTCNT + 1; //Write data for the next slot
        if (mcasp_ptr->ts_idx == mcasp_ptr->num_slots)
        {
            mcasp_ptr->ts_idx = 0;
            mcasp_ptr->buf_cntr++;
            if (mcasp_ptr->buf_cntr == mcasp_ptr->buf_size_words) //Switch buffers
            {
                mcasp_ptr->buf_idx++;
                if (mcasp_ptr->buf_idx == mcasp_ptr->num_bufs_per_slot) mcasp_ptr->buf_idx = 0;
                mcasp_ptr->buf_cntr = 0;
                //!TO DO : Send mailbox with new buf num
            }
        }else if (mcasp_ptr->ts_idx > mcasp_ptr->num_slots) goto tx_error_handling;

        struct buffer * bufs =  (struct buffer *)   (
                                                    (uint32_t)(mcasp_ptr->bufs) +
                                                    ( (sizeof(struct buffer)) * (mcasp_ptr->num_bufs_per_slot) * (mcasp_ptr->ts_idx) ) +
                                                    ( (sizeof(struct buffer)) * (mcasp_ptr->buf_idx) )
                                                    );
#if (TX_RUNTIME_DEBUG)
        bufs->buf[mcasp_ptr->buf_cntr] = (10000 * (mcasp_ptr->ts_idx + 1)) + (100 * (mcasp_ptr->buf_idx + 1)) + mcasp_ptr->buf_cntr;
        #if (DEBUG_LEDS)
            set_industial_2_blue();
        #endif
#endif
        *(mcasp_ptr->MCASP_BUF_ptr) = bufs->buf[mcasp_ptr->buf_cntr];
#if (!BUF_DEBUG || TX_RUNTIME_DEBUG)
        bufs->buf[mcasp_ptr->buf_cntr] = 0x0; /* Clean buffer after send */
#endif
        mcasp_ptr->timeout_cntr = 0; //Clear timeout counter
    }
    else //if (MCASP8_TXSTAT_bits_ptr->DATA)
    {
        mcasp_ptr->timeout_cntr++;
        if (mcasp_ptr->timeout_cntr == XDATA_TIMEOUT) goto tx_error_handling;
    } //end if (MCASP8_TXSTAT_bits_ptr->DATA)

    return;

    tx_error_handling:
            if      (mcasp_ptr->MCASP_STAT_bits_ptr->RN)        set_status_1_red();
            else if (mcasp_ptr->MCASP_STAT_bits_ptr->SYNCERR)   set_industial_2_red();
            else if (mcasp_ptr->MCASP_STAT_bits_ptr->CKFAIL)    set_industial_3_red();
            else                                                set_status_0_red();
    #if DEBUG_MCASP_ERRS
            uint32_t err = mcasp_ptr->MCASP_STAT_bits_ptr->ERR;
    #endif
            mcasp_ptr->init_mcasp();
}

static inline void mcasp_rx ( struct mcasp * mcasp_ptr )
{
    if (mcasp_ptr->MCASP_STAT_bits_ptr->ERR) goto rx_error_handling;
    if (mcasp_ptr->MCASP_STAT_bits_ptr->DATA)
    {
        //This is actual only for <32 timeslot TDM
        if (mcasp_ptr->MCASP_TDMSLOT_bits_ptr->SLOTCNT) //not 0 slot currently in reception
            mcasp_ptr->ts_idx = mcasp_ptr->MCASP_TDMSLOT_bits_ptr->SLOTCNT - 1; //Next slot currently in arrival
        else //O slot currently in reception
            mcasp_ptr->ts_idx = mcasp_ptr->num_slots - 1; //set index to final slot

        if (!mcasp_ptr->ts_idx) //if 0 time slot data in buffer
        {
            mcasp_ptr->buf_cntr++;
            if (mcasp_ptr->buf_cntr == mcasp_ptr->buf_size_words) //Switch buffers
            {
                mcasp_ptr->buf_idx++;
                if (mcasp_ptr->buf_idx == mcasp_ptr->num_bufs_per_slot) mcasp_ptr->buf_idx = 0;
                mcasp_ptr->buf_cntr = 0;
                //!TO DO : Send mailbox with new buf num
            }
        }
        struct buffer * bufs =  (struct buffer *)   (
                                                    (uint32_t)(mcasp_ptr->bufs) +
                                                    ( (sizeof(struct buffer)) * (mcasp_ptr->num_bufs_per_slot) * (mcasp_ptr->ts_idx) ) +
                                                    ( (sizeof(struct buffer)) * (mcasp_ptr->buf_idx) )
                                                    );
        bufs->buf[mcasp_ptr->buf_cntr] = *(mcasp_ptr->MCASP_BUF_ptr);
    }//end if (MCASP8_RXSTAT_bits_ptr->DATA)

    return;

    rx_error_handling:
        if      (mcasp_ptr->MCASP_STAT_bits_ptr->RN)        set_status_1_red();
        else if (mcasp_ptr->MCASP_STAT_bits_ptr->SYNCERR)   set_industial_2_red();
        else if (mcasp_ptr->MCASP_STAT_bits_ptr->CKFAIL)    set_industial_3_red();
        else                                                set_status_0_red();
    #if DEBUG_MCASP_ERRS
        uint32_t err = mcasp_ptr->MCASP_STAT_bits_ptr->ERR;
    #endif
        mcasp_ptr->init_mcasp();
}
#endif

int main(void)
{
#if (!BUFS_ABSTRACTION)
    int i;
    int j;
    int k;
#endif
    /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

    /*--------------------------------------*/
    /*! Initialize McASP8 TX buffers system */
    /*--------------------------------------*/
#if (MCASP8_TX)
    #if (!MCASP_ABSTRACTION)
        uint32_t mcasp8_TX_timeout_ctr  = 0;
        uint32_t tx8_ts_idx             = 0; ///< Timeslot index
        uint32_t tx8_buf_idx            = 0; ///< Index of the buf in time slot bufs array
        uint32_t tx8_buf_cntr           = 0; ///< End point buf member counter
    #endif

    #if (BUFS_ABSTRACTION)
        init_bufs(&mcasp8_tx);
    #else
        uint32_t tx8_bufs_ddr_addr_start = MCASP8_TX_BUFS_START_ADDR; ///TO DO : receive this address from mailbox
        for (i = 0; i < NUM_TX8_SLOTS; i++)
        {
            for (j = 0; j < NUM_TX8_BUFS; j++)
            {
                tx8_bufs[i][j].next      = &tx8_bufs[i][j+1];
                tx8_bufs[i][j].buf       = (uint32_t *) (   tx8_bufs_ddr_addr_start +
                                                        ( i * (BUF_SIZE_BYTES * NUM_TX8_BUFS) ) +
                                                        ( j * BUF_SIZE_BYTES) );
        #if BUF_DEBUG
                tx8_bufs[i][j].buf_num   = j + 1;
                tx8_bufs[i][j].ts_num    = i + 1;
                for (k = 0; k < BUF_SIZE_WORDS; k++)
                {
                #if (ECHO_TEST_TX1_to_RX8)
                    tx8_bufs[i][j].buf[k] = 0x0;
                #else
                    tx8_bufs[i][j].buf[k] = (10000 * (i+1)) + (100 * (j+1)) + (k+1); //numbers to show on output
                #endif
                }
        #endif
            } //end for(j
            tx8_bufs[i][j-1].next = &tx8_bufs[i][0]; //Connect final to the first
        } //end for (i
    #endif
#endif

    /*--------------------------------------*/
    /*! Initialize McASP8 RX buffers system */
    /*--------------------------------------*/
#if (MCASP8_RX)
    #if (!MCASP_ABSTRACTION)
        uint32_t rx8_ts_idx     = 0; ///< Timeslot index
        uint32_t rx8_buf_idx    = 0; ///< Index of the buf in time slot bufs array
        uint32_t rx8_buf_cntr   = 0; ///< End point buf member counter
    #endif
    #if (BUFS_ABSTRACTION)
        init_bufs(&mcasp8_rx);
    #else
        uint32_t rx8_bufs_ddr_addr_start = MCASP8_RX_BUFS_START_ADDR;    ///TO DO : receive this address from mailbox
        for (i = 0; i < NUM_RX8_SLOTS; i++)
        {
            for (j = 0; j < NUM_RX8_BUFS; j++)
            {
                rx8_bufs[i][j].next      = &rx8_bufs[i][j+1];
                rx8_bufs[i][j].buf       = (uint32_t *) (   rx8_bufs_ddr_addr_start +
                                                            ( i * (BUF_SIZE_BYTES * NUM_RX8_BUFS) ) +
                                                            ( j * BUF_SIZE_BYTES) );
        #if BUF_DEBUG
                rx8_bufs[i][j].buf_num   = j + 1;
                rx8_bufs[i][j].ts_num    = i + 1;
        #endif
            } //end for(j
            rx8_bufs[i][j-1].next = &rx8_bufs[i][0]; //Connect final to the first
        } //end for (i
    #endif
#endif

    /*--------------------------------------*/
    /*! Initialize McASP1 TX buffers system */
    /*--------------------------------------*/
#if (MCASP1_TX)
    #if (!MCASP_ABSTRACTION)
        uint32_t mcasp1_TX_timeout_ctr  = 0;
        uint32_t tx1_ts_idx             = 0; ///< Timeslot index
        uint32_t tx1_buf_idx            = 0; ///< Index of the buf in time slot bufs array
        uint32_t tx1_buf_cntr           = 0; ///< End point buf member counter
    #endif
    #if (BUFS_ABSTRACTION)
        init_bufs(&mcasp1_tx);
    #else
        uint32_t tx1_bufs_ddr_addr_start = MCASP1_TX_BUFS_START_ADDR; ///TO DO : receive this address from mailbox
        for (i = 0; i < NUM_TX1_SLOTS; i++)
        {
            for (j = 0; j < NUM_TX1_BUFS; j++)
            {
                tx1_bufs[i][j].next      = &tx1_bufs[i][j+1];
                tx1_bufs[i][j].buf       = (uint32_t *) (   tx1_bufs_ddr_addr_start +
                                                            ( i * (BUF_SIZE_BYTES * NUM_TX1_BUFS) ) +
                                                            ( j * BUF_SIZE_BYTES) );
            #if BUF_DEBUG
                tx1_bufs[i][j].buf_num   = j + 1;
                tx1_bufs[i][j].ts_num    = i + 1;
                for (k = 0; k < BUF_SIZE_WORDS; k++)
                {
                #if (ECHO_TEST_TX8_to_RX1)
                    tx1_bufs[i][j].buf[k] = 0x0;
                #else
                    tx1_bufs[i][j].buf[k] = (10000 * (i+1)) + (100 * (j+1)) + (k+1); //numbers to show on output
                #endif
                }
            #endif
            } //end for(j
            tx1_bufs[i][j-1].next = &tx1_bufs[i][0]; //Connect final to the first
        } //end for (i
    #endif
#endif

    /*--------------------------------------*/
    /*! Initialize McASP1 RX buffers system */
    /*--------------------------------------*/
#if (MCASP1_RX)
    #if (!MCASP_ABSTRACTION)
        uint32_t rx1_ts_idx     = 0;    ///< Timeslot index
        uint32_t rx1_buf_idx    = 0;    ///< Index of the buf in time slot bufs array
        uint32_t rx1_buf_cntr   = 0;    ///< End point buf member counter
    #endif
    #if (BUFS_ABSTRACTION)
        init_bufs(&mcasp1_rx);
    #else
        uint32_t rx1_bufs_ddr_addr_start = MCASP1_RX_BUFS_START_ADDR;    ///TO DO : receive this address from mailbox
        for (i = 0; i < NUM_RX1_SLOTS; i++)
        {
            for (j = 0; j < NUM_RX1_BUFS; j++)
            {
                rx1_bufs[i][j].next      = &rx1_bufs[i][j+1];
                rx1_bufs[i][j].buf       = (uint32_t *) (   rx1_bufs_ddr_addr_start +
                                                            ( i * (BUF_SIZE_BYTES * NUM_RX1_BUFS) ) +
                                                            ( j * BUF_SIZE_BYTES) );
        #if BUF_DEBUG
                rx1_bufs[i][j].buf_num   = j + 1;
                rx1_bufs[i][j].ts_num    = i + 1;
        #endif
            } //end for(j
            rx1_bufs[i][j-1].next = &rx1_bufs[i][0]; //Connect final to the first
        } //end for (i
    #endif
#endif

    clk_cfg();

#if (DEBUG_LEDS)
    setup_gpio_pads();
#endif

#if (MCASP8_TX||MCASP8_RX)
    config_mcasp8_pads();
#endif

#if (MCASP1_TX||MCASP1_RX)
    config_mcasp1_pads();
#endif

    init_mcasp();

    /*----------------------*/
    /*! MAIN PROGRAMM CYCLE */
    /*----------------------*/
    while (1)
    {
        /*------------------*/
        /*! McASP8 TX block */
        /*------------------*/
#if (MCASP8_TX)
    #if (MCASP_ABSTRACTION)
            mcasp_tx(&mcasp8_tx);
    #else
            if (MCASP8_TXSTAT_bits_ptr->ERR) goto tx8_error_handling;
            if (MCASP8_TXSTAT_bits_ptr->DATA)
            {
                tx8_ts_idx = MCASP8_TXTDMSLOT_bits_ptr->SLOTCNT + 1; //Write data for the next slot
                if (tx8_ts_idx == NUM_TX8_SLOTS)
                {
                    tx8_ts_idx = 0;
                    tx8_buf_cntr++;
                    if (tx8_buf_cntr == BUF_SIZE_WORDS) //Flip buffers
                    {
                        tx8_buf_idx++;
                        if (tx8_buf_idx == NUM_TX8_BUFS) tx8_buf_idx = 0;
                        tx8_buf_cntr = 0;
                        //!TO DO : Send mailbox with new buf num
                    }
                }else if (tx8_ts_idx > NUM_TX8_SLOTS)
                    goto tx8_error_handling;
        #if (TX_RUNTIME_DEBUG)
                tx8_bufs[tx8_ts_idx][tx8_buf_idx].buf[tx8_buf_cntr] =  (10000 * (tx8_ts_idx + 1)) + (100 * (tx8_buf_idx + 1)) + tx8_buf_cntr;
        #endif
                * MCASP8_TXBUF_ptr = tx8_bufs[tx8_ts_idx][tx8_buf_idx].buf[tx8_buf_cntr];
        #if (!BUF_DEBUG || TX_RUNTIME_DEBUG || ECHO_TEST_TX1_to_RX8)
                tx8_bufs[tx8_ts_idx][tx8_buf_idx].buf[tx8_buf_cntr] = 0x0; //clear buffer after send
        #endif
                mcasp8_TX_timeout_ctr = 0; //Clear timeout counter
            }
            else //if (MCASP8_TXSTAT_bits_ptr->DATA)
            {
                mcasp8_TX_timeout_ctr++;
                if (mcasp8_TX_timeout_ctr == XDATA_TIMEOUT) goto tx8_error_handling;
            } //end if (MCASP8_TXSTAT_bits_ptr->DATA)
    #endif
#endif
        /*------------------*/
        /*! McASP8 RX block */
        /*------------------*/
#if (MCASP8_RX)
    #if (MCASP_ABSTRACTION)
            mcasp_rx(&mcasp8_rx);
    #else
        if (MCASP8_RXSTAT_bits_ptr->ERR) goto rx8_error_handling;
        if (MCASP8_RXSTAT_bits_ptr->DATA)
        {
            //This is actual only for <32 timeslot TDM
            if (MCASP8_RXTDMSLOT_bits_ptr->SLOTCNT) //not 0 slot currently in reception
                rx8_ts_idx = MCASP8_RXTDMSLOT_bits_ptr->SLOTCNT - 1; //Next slot currently in arrival
            else //O slot currently in reception
                rx8_ts_idx = NUM_RX8_SLOTS - 1;
            if (!rx8_ts_idx)
            {
                rx8_buf_cntr++;
                if (rx8_buf_cntr == BUF_SIZE_WORDS) //Flip buffers
                {
                    rx8_buf_idx++;
                    if (rx8_buf_idx == NUM_RX8_BUFS) rx8_buf_idx = 0;
                    rx8_buf_cntr = 0;
                    //!TO DO : Send mailbox with new buf num
                }
            }
            rx8_bufs[rx8_ts_idx][rx8_buf_idx].buf[rx8_buf_cntr] = * MCASP8_RXBUF_ptr;
        }//end if (MCASP8_RXSTAT_bits_ptr->DATA)
    #endif
#endif
        /*------------------*/
        /*! McASP1 TX block */
        /*------------------*/
#if (MCASP1_TX)
#if (MCASP_ABSTRACTION)
        mcasp_tx(&mcasp1_tx);
    #else
            if (MCASP1_TXSTAT_bits_ptr->ERR) goto tx1_error_handling;
            if (MCASP1_TXSTAT_bits_ptr->DATA)
            {
                tx1_ts_idx = MCASP1_TXTDMSLOT_bits_ptr->SLOTCNT + 1; //Write data for the next slot
                if (tx1_ts_idx == NUM_TX1_SLOTS)
                {
                    tx1_ts_idx = 0;
                    tx1_buf_cntr++;
                    if (tx1_buf_cntr == BUF_SIZE_WORDS) //Flip buffers
                    {
                        tx1_buf_idx++;
                        if (tx1_buf_idx == NUM_TX1_BUFS) tx1_buf_idx = 0;
                        tx1_buf_cntr = 0;
                        //!TO DO : Send mailbox with new buf num
                    }
                }else if (tx1_ts_idx > NUM_TX1_SLOTS)
                    goto tx1_error_handling;
        #if (TX_RUNTIME_DEBUG||ECHO_TEST_TX1_to_RX8)
                tx1_bufs[tx1_ts_idx][tx1_buf_idx].buf[tx1_buf_cntr] = (10000 * (tx1_ts_idx + 1)) + (100 * (tx1_buf_idx + 1)) + tx1_buf_cntr;
        #endif
                * MCASP1_TXBUF_ptr = tx1_bufs[tx1_ts_idx][tx1_buf_idx].buf[tx1_buf_cntr];
        #if (!BUF_DEBUG || TX_RUNTIME_DEBUG)
                tx1_bufs[tx1_ts_idx][tx1_buf_idx].buf[tx1_buf_cntr] = 0x0;  //clear buffer after send
        #endif
                mcasp1_TX_timeout_ctr = 0; //Clear timeout counter
            }
            else //if (MCASP1_TXSTAT_bits_ptr->DATA)
            {
                mcasp1_TX_timeout_ctr++;
                if (mcasp1_TX_timeout_ctr == XDATA_TIMEOUT) goto tx1_error_handling;
            } //end if (MCASP1_TXSTAT_bits_ptr->DATA)
    #endif
#endif
        /*------------------*/
        /*! McASP1 RX block */
        /*------------------*/
#if (MCASP1_RX)
    #if (MCASP_ABSTRACTION)
        mcasp_rx(&mcasp1_rx);
    #else
        if (MCASP1_RXSTAT_bits_ptr->ERR) goto rx1_error_handling;
        if (MCASP1_RXSTAT_bits_ptr->DATA)
        {
            set_industial_3_blue();
            //This is actual only for <32 timeslot TDM
            if (MCASP1_RXTDMSLOT_bits_ptr->SLOTCNT) //not 0 slot currently in reception
                rx1_ts_idx = MCASP1_RXTDMSLOT_bits_ptr->SLOTCNT - 1; //Next slot currently in arrival
            else //O slot currently in reception
                rx1_ts_idx = NUM_RX8_SLOTS - 1;
            if (!rx1_ts_idx)
            {
                rx1_buf_cntr++;
                if (rx1_buf_cntr == BUF_SIZE_WORDS) //Flip buffers
                {
                    rx1_buf_idx++;
                    if (rx1_buf_idx == NUM_RX1_BUFS) rx1_buf_idx = 0;
                    rx1_buf_cntr = 0;
                    //!TO DO : Send mailbox with new buf num
                }
            }
            rx1_bufs[rx1_ts_idx][rx1_buf_idx].buf[rx1_buf_cntr] = * MCASP1_RXBUF_ptr;
        }//end if (MCASP1_RXSTAT_bits_ptr->DATA)
    #endif
#endif

        continue;

#if DEBUG_MCASP_ERRS
        uint32_t err = 0;
#endif
#if(!MCASP_ABSTRACTION)
    #if (MCASP8_TX)
        tx8_error_handling:
    #if DEBUG_MCASP_ERRS
            err = MCASP8_TXSTAT_bits_ptr->ERR;
    #endif
            set_status_0_red();
            init_mcasp8();
            continue;
    #endif

    #if (MCASP8_RX)
        rx8_error_handling:
    #if DEBUG_MCASP_ERRS
            err = MCASP8_TXSTAT_bits_ptr->ERR;
    #endif
            set_status_1_red();
            init_mcasp8();
            continue;
    #endif


    #if (MCASP1_TX)
        tx1_error_handling:
    #if DEBUG_MCASP_ERRS
            err = MCASP8_TXSTAT_bits_ptr->ERR;
    #endif
            set_industial_2_red();
            init_mcasp1();
            continue;
    #endif


    #if (MCASP1_RX)
        rx1_error_handling:
    #if DEBUG_MCASP_ERRS
            err = MCASP8_TXSTAT_bits_ptr->ERR;
    #endif
            set_industial_3_red();
            init_mcasp1();
            continue;
    #endif
#endif
    }
}
