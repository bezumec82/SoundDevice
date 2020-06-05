#include "mcasp_init.h"
#if (MCASP8_TX||MCASP8_RX)

/*---------*/
/* McASP 8 */
/*---------*/

/* McASP 8 common setup */
struct MCASP_GBLCTL *       MCASP8_GBLCTL_bits_ptr      = (struct MCASP_GBLCTL *)   (MCASP8_CFG_ADDR + MCASP_GBLCTL_OFF);
uint32_t *                  MCASP8_GBLCTL_ptr           = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_GBLCTL_OFF);
/* When writing to this register, only the TRANSMIT bits of GBLCTL are affected */
struct MCASP_GBLCTL *       MCASP8_GBLCTLX_bits_ptr     = (struct MCASP_GBLCTL *)   (MCASP8_CFG_ADDR + MCASP_GBLCTLX_OFF);
uint32_t *                  MCASP8_GBLCTLX_ptr          = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_GBLCTLX_OFF);
/* When writing to this register, only the RECEIVE bits of GBLCTL are affected */
struct MCASP_GBLCTL *       MCASP8_GBLCTLR_bits_ptr     = (struct MCASP_GBLCTL *)   (MCASP8_CFG_ADDR + MCASP_GBLCTLR_OFF);
uint32_t *                  MCASP8_GBLCTLR_ptr          = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_GBLCTLR_OFF);

uint32_t *                  MCASP8_LBCTL_ptr            = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_LBCTL_OFF);
uint32_t *                  MCASP8_TXDITCTL_ptr         = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_TXDITCTL_OFF);

uint32_t *                  MCASP8_IRQCTLX              = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_IRQCTLX_OFF);
uint32_t *                  MCASP8_IRQCTLR              = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_IRQCTLR_OFF);

struct MCASP_DMACTL *       MCASP8_DMACTLX_bits_ptr     = (struct MCASP_DMACTL *)   (MCASP8_CFG_ADDR + MCASP_DMACTLX_OFF);
struct MCASP_DMACTL *       MCASP8_DMACTLR_bits_ptr     = (struct MCASP_DMACTL *)   (MCASP8_CFG_ADDR + MCASP_DMACTLR_OFF);

/* McASP 8 pad setup */
struct MCASP_PFUNC *        MCASP8_PFUNC_bits_ptr       = (struct MCASP_PFUNC *)    (MCASP8_CFG_ADDR + MCASP_PFUNC_OFF);
uint32_t *                  MCASP8_PFUNC_ptr            = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_PFUNC_OFF);

struct MCASP_PDIR *         MCASP8_PDIR_bits_ptr        = (struct MCASP_PDIR *)     (MCASP8_CFG_ADDR + MCASP_PDIR_OFF);

/* McASP 8 xmission setup */
uint32_t *                  MCASP8_TXMASK               = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_TXMASK_OFF);
uint32_t *                  MCASP8_RXMASK               = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_RXMASK_OFF);

uint32_t *                  MCASP8_TXBUF_ptr            = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_TXBUF_00_OFF);
uint32_t *                  MCASP8_RXBUF_ptr            = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_RXBUF_01_OFF);

/* McASP 8 clock setup */
struct MCASP_AHCLKCTL *     MCASP8_AHCLKXCTL_bits_ptr   = (struct MCASP_AHCLKCTL *) (MCASP8_CFG_ADDR + MCASP_AHCLKXCTL_OFF);
struct MCASP_AHCLKCTL *     MCASP8_AHCLKRCTL_bits_ptr   = (struct MCASP_AHCLKCTL *) (MCASP8_CFG_ADDR + MCASP_AHCLKRCTL_OFF);

struct MCASP_ACLKCTL *      MCASP8_ACLKXCTL_bits_ptr    = (struct MCASP_ACLKCTL *)  (MCASP8_CFG_ADDR + MCASP_ACLKXCTL_OFF);
struct MCASP_ACLKCTL *      MCASP8_ACLKRCTL_bits_ptr    = (struct MCASP_ACLKCTL *)  (MCASP8_CFG_ADDR + MCASP_ACLKRCTL_OFF);

struct MCASP_CLKCHK *       MCASP8_TXCLKCHK_bits_ptr    = (struct MCASP_CLKCHK *)   (MCASP8_CFG_ADDR + MCASP_TXCLKCHK_OFF);
struct MCASP_CLKCHK *       MCASP8_RXCLKCHK_bits_ptr    = (struct MCASP_CLKCHK *)   (MCASP8_CFG_ADDR + MCASP_RXCLKCHK_OFF);

/* McASP8 frame setup */
struct MCASP_FMCTL *        MCASP8_TXFMCTL_bits_ptr     = (struct MCASP_FMCTL *)    (MCASP8_CFG_ADDR + MCASP_TXFMCTL_OFF);
struct MCASP_FMCTL *        MCASP8_RXFMCTL_bits_ptr     = (struct MCASP_FMCTL *)    (MCASP8_CFG_ADDR + MCASP_RXFMCTL_OFF);

struct MCASP_XFMT *         MCASP8_TXFMT_bits_ptr       = (struct MCASP_XFMT *)     (MCASP8_CFG_ADDR + MCASP_TXFMT_OFF);
struct MCASP_XFMT *         MCASP8_RXFMT_bits_ptr       = (struct MCASP_XFMT *)     (MCASP8_CFG_ADDR + MCASP_RXFMT_OFF);

struct MCASP_TDMS *         MCASP8_TXTDM_bits_ptr       = (struct MCASP_TDMS *)     (MCASP8_CFG_ADDR + MCASP_TXTDM_OFF);
uint32_t *                  MCASP8_TXTDM_ptr            = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_TXTDM_OFF);
struct MCASP_TDMS *         MCASP8_RXTDM_bits_ptr       = (struct MCASP_TDMS *)     (MCASP8_CFG_ADDR + MCASP_RXTDM_OFF);
uint32_t *                  MCASP8_RXTDM_ptr            = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_RXTDM_OFF);

/* McASP8 flags */
struct MCASP_STAT *         MCASP8_TXSTAT_bits_ptr      = (struct MCASP_STAT *)     (MCASP8_CFG_ADDR + MCASP_TXSTAT_OFF);
uint32_t *                  MCASP8_TXSTAT_ptr           = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_TXSTAT_OFF);
struct MCASP_STAT *         MCASP8_RXSTAT_bits_ptr      = (struct MCASP_STAT *)     (MCASP8_CFG_ADDR + MCASP_RXSTAT_OFF);
uint32_t *                  MCASP8_RXSTAT_ptr           = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_RXSTAT_OFF);

struct MCASP_TDMSLOT *      MCASP8_TXTDMSLOT_bits_ptr   = (struct MCASP_TDMSLOT *)  (MCASP8_CFG_ADDR + MCASP_TXTDMSLOT_OFF);
struct MCASP_TDMSLOT *      MCASP8_RXTDMSLOT_bits_ptr   = (struct MCASP_TDMSLOT *)  (MCASP8_CFG_ADDR + MCASP_RXTDMSLOT_OFF);

/* McASP8 serializers control */
struct MCASP_SRCTL *        MCASP8_SRCTL_TX_bits_ptr    = (struct MCASP_SRCTL *)    (MCASP8_CFG_ADDR + MCASP_XRSRCTL_00_OFF);
struct MCASP_SRCTL *        MCASP8_SRCTL_RX_bits_ptr    = (struct MCASP_SRCTL *)    (MCASP8_CFG_ADDR + MCASP_XRSRCTL_01_OFF);

uint32_t *                  MCASP8_SRCTL_TX_ptr         = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_XRSRCTL_00_OFF);
uint32_t *                  MCASP8_SRCTL_RX_ptr         = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_XRSRCTL_01_OFF);
uint32_t *                  MCASP8_XRSRCTL_02_ptr       = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_XRSRCTL_02_OFF);
uint32_t *                  MCASP8_XRSRCTL_03_ptr       = (uint32_t *)              (MCASP8_CFG_ADDR + MCASP_XRSRCTL_03_OFF);

static inline void setup_mcasp8_pads (void)
{
#if (MCASP8_TX||MCASP8_RX)
  * MCASP8_PFUNC_ptr = 0xffffffff;
    MCASP8_PFUNC_bits_ptr->AXR0          = 0x0;
    MCASP8_PFUNC_bits_ptr->AXR1          = 0x0;
    MCASP8_PFUNC_bits_ptr->ACLKX         = 0x0;
    MCASP8_PFUNC_bits_ptr->AFSX          = 0x0;
    //Set pin direction
    MCASP8_PDIR_bits_ptr->AXR0           = 0x1; //out
    MCASP8_PDIR_bits_ptr->AXR1           = 0x0; //in
    MCASP8_PDIR_bits_ptr->ACLKX          = 0x1; //out
    #if ECHO_TEST_TX1_to_RX8
        MCASP8_PDIR_bits_ptr->AFSX           = 0x0; //in
    #else
        MCASP8_PDIR_bits_ptr->AFSX           = 0x1; //out
    #endif
#endif
}
/*--- End of the function ----------------------------------------------------*/

static inline void setup_mcasp8_TX (void)
{
#if (MCASP8_TX)
start:
    //Reset TX part
    * MCASP8_GBLCTLX_ptr = 0x0;
    WAIT_REG_RESET(* MCASP8_GBLCTLX_ptr);
    //Switch off TX serializer
    * MCASP8_SRCTL_TX_ptr               = 0x0;
    //Switch off all TX event generation
    * MCASP8_IRQCTLX                    = 0x0; //All IRQs are disabled
    MCASP8_DMACTLX_bits_ptr->DATDMA     = 0x1; //Disable DMA requests generation
    //Clear TX stat
    * MCASP8_TXSTAT_ptr                 = 0xffffffff;
    //TX high speed clock
    MCASP8_AHCLKXCTL_bits_ptr->HCLKDIV  = 100-1; /* PER_ABE_X1_GFCLK -> MCASP8_AUX_GFCLK = 100 MHz */
    MCASP8_AHCLKXCTL_bits_ptr->HCLKP    = 0x1; //0 == Rising 1 == Falling
    MCASP8_AHCLKXCTL_bits_ptr->HCLKM    = 0x1; //Internal AHCLKX
    //TX clock setup
    MCASP8_ACLKXCTL_bits_ptr->CLKDIV    = 0x0;
    MCASP8_ACLKXCTL_bits_ptr->CLKM      = 0x1; //Internal (from divider)
    MCASP8_ACLKXCTL_bits_ptr->ASYNC     = 0x0; //Synchronous
    MCASP8_ACLKXCTL_bits_ptr->CLKP      = 0x1; //0 == Rising 1 == Falling
    //TX clock control
    MCASP8_TXCLKCHK_bits_ptr->MIN       = 0x0;
    MCASP8_TXCLKCHK_bits_ptr->MAX       = 0xff;
    MCASP8_TXCLKCHK_bits_ptr->PS        = 0x8; //McASP interface clock divided by 256
    //TX frame format
    MCASP8_TXFMCTL_bits_ptr->FSP        = 0x1; //start of the frame : 1 == falling edge 0 == rising edge
#if ECHO_TEST_TX1_to_RX8
    MCASP8_TXFMCTL_bits_ptr->FSM        = 0x0; //Externally
#else
    MCASP8_TXFMCTL_bits_ptr->FSM        = 0x1; //Internally
#endif
    MCASP8_TXFMCTL_bits_ptr->FWID       = 0x1; //Word wide frame
    MCASP8_TXFMCTL_bits_ptr->MOD        = 0x2; //I2S mode
    //TX data format
    MCASP8_TXFMT_bits_ptr->ROT          = 0x4; //0x4 == 16 bit rotation
    MCASP8_TXFMT_bits_ptr->BUSEL        = 0x1; //1 == CFG bus
    MCASP8_TXFMT_bits_ptr->SSZ          = 0xf; //32 bit per slot
    MCASP8_TXFMT_bits_ptr->RVRS         = 0x1; //1 == MSB first
    MCASP8_TXFMT_bits_ptr->DATDLY       = 0x1; //delay in bits
    //Set TX data mask
#if ECHO_TEST_TX1_to_RX8
    * MCASP8_TXMASK = 0xffffffff;
#else
    * MCASP8_TXMASK = 0x0000ffff; /* Last 16 bit matter */
#endif
    //Active slots
    * MCASP8_TXTDM_ptr                  = 0x0;
    MCASP8_TXTDM_bits_ptr->TDMS0        = 0x1;
    MCASP8_TXTDM_bits_ptr->TDMS1        = 0x1;
    //Setup serializers
    MCASP8_SRCTL_TX_bits_ptr->SRMOD     = 0x1; //transmitter
    MCASP8_SRCTL_TX_bits_ptr->DISMOD    = 0x0; //tri-state
#endif
    }
/*--- End of the function ----------------------------------------------------*/

static inline void setup_mcasp8_RX (void)
{
#if (MCASP8_RX)
start:
    //Reset RX
    * MCASP8_GBLCTLR_ptr = 0x0;
    WAIT_REG_RESET(* MCASP8_GBLCTLR_ptr);

    //Switch off RX serializer
    * MCASP8_SRCTL_RX_ptr               = 0x0;

    //Switch off all RX event generation
    * MCASP8_IRQCTLR                    = 0x0; //All IRQs are disabled
    MCASP8_DMACTLR_bits_ptr->DATDMA     = 0x1; //Disable DMA requests generation

    //Clear RX stat
    * MCASP8_RXSTAT_ptr                 = 0xffffffff;

    //RX high speed clock
    //Cause MCASP8_ACLKXCTL_bits_ptr->ASYNC == 0x0
    MCASP8_AHCLKRCTL_bits_ptr->HCLKDIV  = 4095;
    MCASP8_AHCLKRCTL_bits_ptr->HCLKP    = 0x0; //0 == Rising 1 == Falling
    MCASP8_AHCLKRCTL_bits_ptr->HCLKM    = 0x0; //1 == internal 0 == external

    //RX clock setup
    MCASP8_ACLKRCTL_bits_ptr->CLKDIV    = 0x0;
    MCASP8_ACLKRCTL_bits_ptr->CLKM      = 0x0; //1 == internal 0 == external
    MCASP8_ACLKRCTL_bits_ptr->CLKP      = 0x0; //0 == Rising 1 == Falling

    //RX clock control
    MCASP8_RXCLKCHK_bits_ptr->MIN       = 0x0;
    MCASP8_RXCLKCHK_bits_ptr->MAX       = 0xff;
    MCASP8_RXCLKCHK_bits_ptr->PS        = 0x8; //McASP interface clock divided by 256

    //RX frame format
    MCASP8_RXFMCTL_bits_ptr->FSP        = 0x1; //start of the frame : 1 == falling edge 0 == rising edge
#if ECHO_TEST_TX1_to_RX8
    MCASP8_RXFMCTL_bits_ptr->FSM        = 0x0; //Externally
#else
    MCASP8_RXFMCTL_bits_ptr->FSM        = 0x1; //Internally
#endif
    MCASP8_RXFMCTL_bits_ptr->FWID       = 0x1; //Word wide frame
    MCASP8_RXFMCTL_bits_ptr->MOD        = 0x2; //I2S mode

    //RX data format
    MCASP8_RXFMT_bits_ptr->ROT          = 0x4; //0x0 == no rotation 0x4 == 16 bit rotation
    MCASP8_RXFMT_bits_ptr->BUSEL        = 0x1; //1 == CFG bus
    MCASP8_RXFMT_bits_ptr->SSZ          = 0xf; //32 bit per slot
    MCASP8_RXFMT_bits_ptr->RVRS         = 0x1; //1 == MSB first
    MCASP8_RXFMT_bits_ptr->DATDLY       = 0x1; //delay in bits

    //Set data mask
#if ECHO_TEST_TX1_to_RX8
    * MCASP8_RXMASK = 0xffffffff;
#else
    * MCASP8_RXMASK = 0x0000ffff; /* Last 16 bit matter */
#endif
    //Active slots
    * MCASP8_RXTDM_ptr                  = 0x0;
    MCASP8_RXTDM_bits_ptr->TDMS0        = 0x1;
    MCASP8_RXTDM_bits_ptr->TDMS1        = 0x1;

    //Setup serializers
    MCASP8_SRCTL_RX_bits_ptr->SRMOD     = 0x2; //receiver
    MCASP8_SRCTL_RX_bits_ptr->DISMOD    = 0x0; //tri-state
#endif
}
/*--- End of the function ----------------------------------------------------*/

static int32_t start_mcasp8_TX (void)
{
#if (MCASP8_TX)
    uint32_t timeout_counter = 0;

    /** Start TX high-speed clock */
    MCASP8_GBLCTLX_bits_ptr->XHCLKRST = 0x1;
    while (!MCASP8_GBLCTLX_bits_ptr->XHCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
            set_status_0_red();
            return -1;
        }
    };

    /** Start TX clock */
    MCASP8_GBLCTLX_bits_ptr->XCLKRST = 0x1;
    timeout_counter = 0;
    while (!MCASP8_GBLCTLX_bits_ptr->XCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
            set_status_0_red();
            return -1;
        }
    };

    //Clear stat
    * MCASP8_TXSTAT_ptr = 0xffffffff;

    /** Flush TX buffers to an empty state */
    MCASP8_GBLCTLX_bits_ptr->XSRCLR = 0x1;
    timeout_counter = 0;
    while (!MCASP8_GBLCTLX_bits_ptr->XSRCLR)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
            set_status_0_red();
            return -1;
        }
    };

    MCASP8_TXSTAT_bits_ptr->DATA = 0x1;

    /** Start TX state machine */
    MCASP8_GBLCTLX_bits_ptr->XSMRST = 0x1;
    timeout_counter = 0;
    while (!MCASP8_GBLCTLX_bits_ptr->XSMRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
            set_status_0_red();
            return -1;
        }
    };

    //Write to the TX buffer to prevent underflow
    * MCASP8_TXBUF_ptr = 0x0;
    * MCASP8_TXBUF_ptr = 0x0;

    /** Start TX frame sync generator */
    MCASP8_GBLCTLX_bits_ptr->XFRST = 0x1;
    timeout_counter = 0;
    while (!MCASP8_GBLCTLX_bits_ptr->XFRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
            set_status_0_red();
            return -1;
        }
    };
#endif
    return 0;
}
/*--- End of the function ----------------------------------------------------*/

static int32_t start_mcasp8_RX (void)
{
#if (MCASP8_RX)
    uint32_t timeout_counter = 0;

    /** Start RX high-speed clock */
    MCASP8_GBLCTLR_bits_ptr->RHCLKRST = 0x1;
    while (!MCASP8_GBLCTLR_bits_ptr->RHCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
            set_status_1_red();
            return -1;
        }
    };

    /** Start RX clock */
    MCASP8_GBLCTLR_bits_ptr->RCLKRST = 0x1;
    timeout_counter = 0;
    while (!MCASP8_GBLCTLR_bits_ptr->RCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
            set_status_1_red();
            return -1;
        }
    };

    //Clear stat
    * MCASP8_RXSTAT_ptr = 0xffffffff;

    /** Flush RX buffers to an empty state */
    MCASP8_GBLCTLR_bits_ptr->RSRCLR = 0x1;
    timeout_counter = 0;
    while (!MCASP8_GBLCTLR_bits_ptr->RSRCLR)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
            set_status_1_red();
            return -1;
        }
    };

    MCASP8_RXSTAT_bits_ptr->DATA = 0x1;

    /** Start RX state machine */
    MCASP8_GBLCTLR_bits_ptr->RSMRST = 0x1;
    timeout_counter = 0;
    while (!MCASP8_GBLCTLR_bits_ptr->RSMRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
            set_status_1_red();
            return -1;
        }
    };

    //Read the RX buffer to prevent underflow
    uint32_t    dump = * MCASP8_RXBUF_ptr;
                dump = * MCASP8_RXBUF_ptr;

    /** Start frame sync generator */
    MCASP8_GBLCTLR_bits_ptr->RFRST = 0x1;
    timeout_counter = 0;
    while (!MCASP8_GBLCTLR_bits_ptr->RFRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
            set_status_1_red();
            return -1;
        }
    };
#endif
    return 0;
}
/*--- End of the function ----------------------------------------------------*/

void init_mcasp8 (void){
    uint32_t pid8 = 0;
    pid8 = *(uint32_t *)MCASP8_CFG_ADDR; //If 0x0 here - no clk

start:
#if (MCASP8_TX||MCASP8_RX)
    /** Disable McASP */
    //Place controller in reset state
    * MCASP8_GBLCTL_ptr = 0;
    WAIT_REG_RESET(* MCASP8_GBLCTL_ptr);
    //Switch off all the serializes
    * MCASP8_SRCTL_TX_ptr     = 0x0;
    * MCASP8_SRCTL_RX_ptr     = 0x0;
    * MCASP8_XRSRCTL_02_ptr   = 0x0;
    * MCASP8_XRSRCTL_03_ptr   = 0x0;

    /** Global McASP8 setup */
    * MCASP8_LBCTL_ptr      = 0x0; //No loop-back
    * MCASP8_TXDITCTL_ptr   = 0x0; //No DIT
#endif

    setup_mcasp8_TX();
    setup_mcasp8_RX();

    setup_mcasp8_pads();

    if (start_mcasp8_TX() < 0) goto start;
    if (start_mcasp8_RX() < 0) goto start;
}
/*--- End of the function ----------------------------------------------------*/

#endif
