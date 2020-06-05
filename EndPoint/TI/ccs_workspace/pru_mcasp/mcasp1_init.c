#include "mcasp_init.h"
#if (MCASP1_TX||MCASP1_RX)

/*---------*/
/* McASP 1 */
/*---------*/

/* McASP 1 common setup */
struct MCASP_GBLCTL *       MCASP1_GBLCTL_bits_ptr      = (struct MCASP_GBLCTL *)   (MCASP1_CFG_ADDR + MCASP_GBLCTL_OFF);
uint32_t *                  MCASP1_GBLCTL_ptr           = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_GBLCTL_OFF);
/* When writing to this register, only the TRANSMIT bits of GBLCTL are affected */
struct MCASP_GBLCTL *       MCASP1_GBLCTLX_bits_ptr     = (struct MCASP_GBLCTL *)   (MCASP1_CFG_ADDR + MCASP_GBLCTLX_OFF);
uint32_t *                  MCASP1_GBLCTLX_ptr          = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_GBLCTLX_OFF);
/* When writing to this register, only the RECEIVE bits of GBLCTL are affected */
struct MCASP_GBLCTL *       MCASP1_GBLCTLR_bits_ptr     = (struct MCASP_GBLCTL *)   (MCASP1_CFG_ADDR + MCASP_GBLCTLR_OFF);
uint32_t *                  MCASP1_GBLCTLR_ptr          = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_GBLCTLR_OFF);

uint32_t *                  MCASP1_LBCTL_ptr            = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_LBCTL_OFF);
uint32_t *                  MCASP1_TXDITCTL_ptr         = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_TXDITCTL_OFF);

uint32_t *                  MCASP1_IRQCTLX              = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_IRQCTLX_OFF);
uint32_t *                  MCASP1_IRQCTLR              = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_IRQCTLR_OFF);

struct MCASP_DMACTL *       MCASP1_DMACTLX_bits_ptr     = (struct MCASP_DMACTL *)   (MCASP1_CFG_ADDR + MCASP_DMACTLX_OFF);
struct MCASP_DMACTL *       MCASP1_DMACTLR_bits_ptr     = (struct MCASP_DMACTL *)   (MCASP1_CFG_ADDR + MCASP_DMACTLR_OFF);

/* McASP 1 pad setup */
struct MCASP_PFUNC *        MCASP1_PFUNC_bits_ptr       = (struct MCASP_PFUNC *)    (MCASP1_CFG_ADDR + MCASP_PFUNC_OFF);
uint32_t *                  MCASP1_PFUNC_ptr            = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_PFUNC_OFF);

struct MCASP_PDIR *         MCASP1_PDIR_bits_ptr        = (struct MCASP_PDIR *)     (MCASP1_CFG_ADDR + MCASP_PDIR_OFF);

/* McASP 1 xmission setup */
uint32_t *                  MCASP1_TXMASK               = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_TXMASK_OFF);
uint32_t *                  MCASP1_RXMASK               = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_RXMASK_OFF);

uint32_t *                  MCASP1_TXBUF_ptr            = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_TXBUF_08_OFF);
uint32_t *                  MCASP1_RXBUF_ptr            = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_RXBUF_14_OFF);

/* McASP 1 clock setup */
struct MCASP_AHCLKCTL *     MCASP1_AHCLKXCTL_bits_ptr   = (struct MCASP_AHCLKCTL *) (MCASP1_CFG_ADDR + MCASP_AHCLKXCTL_OFF);
struct MCASP_AHCLKCTL *     MCASP1_AHCLKRCTL_bits_ptr   = (struct MCASP_AHCLKCTL *) (MCASP1_CFG_ADDR + MCASP_AHCLKRCTL_OFF);

struct MCASP_ACLKCTL *      MCASP1_ACLKXCTL_bits_ptr    = (struct MCASP_ACLKCTL *)  (MCASP1_CFG_ADDR + MCASP_ACLKXCTL_OFF);
struct MCASP_ACLKCTL *      MCASP1_ACLKRCTL_bits_ptr    = (struct MCASP_ACLKCTL *)  (MCASP1_CFG_ADDR + MCASP_ACLKRCTL_OFF);

struct MCASP_CLKCHK *       MCASP1_TXCLKCHK_bits_ptr    = (struct MCASP_CLKCHK *)   (MCASP1_CFG_ADDR + MCASP_TXCLKCHK_OFF);
struct MCASP_CLKCHK *       MCASP1_RXCLKCHK_bits_ptr    = (struct MCASP_CLKCHK *)   (MCASP1_CFG_ADDR + MCASP_RXCLKCHK_OFF);

/* McASP1 frame setup */
struct MCASP_FMCTL *        MCASP1_TXFMCTL_bits_ptr     = (struct MCASP_FMCTL *)    (MCASP1_CFG_ADDR + MCASP_TXFMCTL_OFF);
struct MCASP_FMCTL *        MCASP1_RXFMCTL_bits_ptr     = (struct MCASP_FMCTL *)    (MCASP1_CFG_ADDR + MCASP_RXFMCTL_OFF);

struct MCASP_XFMT *         MCASP1_TXFMT_bits_ptr       = (struct MCASP_XFMT *)     (MCASP1_CFG_ADDR + MCASP_TXFMT_OFF);
struct MCASP_XFMT *         MCASP1_RXFMT_bits_ptr       = (struct MCASP_XFMT *)     (MCASP1_CFG_ADDR + MCASP_RXFMT_OFF);

struct MCASP_TDMS *         MCASP1_TXTDM_bits_ptr       = (struct MCASP_TDMS *)     (MCASP1_CFG_ADDR + MCASP_TXTDM_OFF);
uint32_t *                  MCASP1_TXTDM_ptr            = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_TXTDM_OFF);
struct MCASP_TDMS *         MCASP1_RXTDM_bits_ptr       = (struct MCASP_TDMS *)     (MCASP1_CFG_ADDR + MCASP_RXTDM_OFF);
uint32_t *                  MCASP1_RXTDM_ptr            = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_RXTDM_OFF);

/* McASP1 flags */
struct MCASP_STAT *         MCASP1_TXSTAT_bits_ptr      = (struct MCASP_STAT *)     (MCASP1_CFG_ADDR + MCASP_TXSTAT_OFF);
uint32_t *                  MCASP1_TXSTAT_ptr           = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_TXSTAT_OFF);
struct MCASP_STAT *         MCASP1_RXSTAT_bits_ptr      = (struct MCASP_STAT *)     (MCASP1_CFG_ADDR + MCASP_RXSTAT_OFF);
uint32_t *                  MCASP1_RXSTAT_ptr           = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_RXSTAT_OFF);

struct MCASP_TDMSLOT *      MCASP1_TXTDMSLOT_bits_ptr   = (struct MCASP_TDMSLOT *)  (MCASP1_CFG_ADDR + MCASP_TXTDMSLOT_OFF);
struct MCASP_TDMSLOT *      MCASP1_RXTDMSLOT_bits_ptr   = (struct MCASP_TDMSLOT *)  (MCASP1_CFG_ADDR + MCASP_RXTDMSLOT_OFF);

/* McASP1 serializers control */
struct MCASP_SRCTL *        MCASP1_SRCTL_TX_bits_ptr    = (struct MCASP_SRCTL *)    (MCASP1_CFG_ADDR + MCASP_XRSRCTL_08_OFF);
struct MCASP_SRCTL *        MCASP1_SRCTL_RX_bits_ptr    = (struct MCASP_SRCTL *)    (MCASP1_CFG_ADDR + MCASP_XRSRCTL_14_OFF);

uint32_t *                  MCASP1_SRCTL_TX_ptr         = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_08_OFF);
uint32_t *                  MCASP1_SRCTL_RX_ptr         = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_14_OFF);
uint32_t *                  MCASP1_XRSRCTL_00_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_00_OFF);
uint32_t *                  MCASP1_XRSRCTL_01_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_01_OFF);
uint32_t *                  MCASP1_XRSRCTL_02_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_02_OFF);
uint32_t *                  MCASP1_XRSRCTL_03_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_03_OFF);
uint32_t *                  MCASP1_XRSRCTL_04_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_04_OFF);
uint32_t *                  MCASP1_XRSRCTL_05_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_05_OFF);
uint32_t *                  MCASP1_XRSRCTL_06_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_06_OFF);
uint32_t *                  MCASP1_XRSRCTL_07_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_07_OFF);

uint32_t *                  MCASP1_XRSRCTL_09_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_09_OFF);
uint32_t *                  MCASP1_XRSRCTL_10_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_10_OFF);
uint32_t *                  MCASP1_XRSRCTL_11_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_11_OFF);
uint32_t *                  MCASP1_XRSRCTL_12_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_12_OFF);
uint32_t *                  MCASP1_XRSRCTL_13_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_13_OFF);

uint32_t *                  MCASP1_XRSRCTL_15_ptr       = (uint32_t *)              (MCASP1_CFG_ADDR + MCASP_XRSRCTL_15_OFF);


void setup_mcasp1_pads (void)
{
#if (MCASP1_TX||MCASP1_RX)
  * MCASP1_PFUNC_ptr = 0xffffffff;
    MCASP1_PFUNC_bits_ptr->AXR8          = 0x0;
    MCASP1_PFUNC_bits_ptr->AXR14         = 0x0;
    MCASP1_PFUNC_bits_ptr->ACLKX         = 0x0;
    MCASP1_PFUNC_bits_ptr->AFSX          = 0x0;
    //Set pin direction
    MCASP1_PDIR_bits_ptr->AXR8           = 0x1; //out
    MCASP1_PDIR_bits_ptr->AXR14          = 0x0; //in
    MCASP1_PDIR_bits_ptr->ACLKX          = 0x1; //out
    MCASP1_PDIR_bits_ptr->AFSX           = 0x1; //out
#endif
}
/*--- End of the function ----------------------------------------------------*/

void setup_mcasp1_TX (void)
{
#if (MCASP1_TX)
start:
    //Reset TX part
    * MCASP1_GBLCTLX_ptr = 0x0;
    WAIT_REG_RESET(* MCASP1_GBLCTLX_ptr);
    //Switch off TX serializer
    * MCASP1_SRCTL_TX_ptr               = 0x0;
    //Switch off all TX event generation
    * MCASP1_IRQCTLX                    = 0x0; //All IRQs are disabled
    MCASP1_DMACTLX_bits_ptr->DATDMA     = 0x1; //Disable DMA requests generation
    //Clear TX stat
    * MCASP1_TXSTAT_ptr                 = 0xffffffff;
    //TX high speed clock
    MCASP1_AHCLKXCTL_bits_ptr->HCLKDIV  = 100-1; /* PER_ABE_X1_GFCLK -> MCASP1_AUX_GFCLK = 100 MHz */
    MCASP1_AHCLKXCTL_bits_ptr->HCLKP    = 0x1; //0 == Rising 1 == Falling
    MCASP1_AHCLKXCTL_bits_ptr->HCLKM    = 0x1; //Internal AHCLKX
    //TX clock setup
    MCASP1_ACLKXCTL_bits_ptr->CLKDIV    = 0x0;
    MCASP1_ACLKXCTL_bits_ptr->CLKM      = 0x1; //Internal (from divider)
    MCASP1_ACLKXCTL_bits_ptr->ASYNC     = 0x0; //Synchronous
    MCASP1_ACLKXCTL_bits_ptr->CLKP      = 0x1; //0 == Rising 1 == Falling
    //TX clock control
    MCASP1_TXCLKCHK_bits_ptr->MIN       = 0x0;
    MCASP1_TXCLKCHK_bits_ptr->MAX       = 0xff;
    MCASP1_TXCLKCHK_bits_ptr->PS        = 0x8; //McASP interface clock divided by 256
    //TX frame format
    MCASP1_TXFMCTL_bits_ptr->FSP        = 0x1; //start of the frame : 1 == falling edge 0 == rising edge
    MCASP1_TXFMCTL_bits_ptr->FSM        = 0x1; //Internally
    MCASP1_TXFMCTL_bits_ptr->FWID       = 0x1; //Word wide frame
    MCASP1_TXFMCTL_bits_ptr->MOD        = 0x2; //I2S mode
    //TX data format
    MCASP1_TXFMT_bits_ptr->ROT          = 0x4; //0x4 == 16 bit rotation
    MCASP1_TXFMT_bits_ptr->BUSEL        = 0x1; //1 == CFG bus
    MCASP1_TXFMT_bits_ptr->SSZ          = 0xf; //32 bit per slot
    MCASP1_TXFMT_bits_ptr->RVRS         = 0x1; //1 == MSB first
    MCASP1_TXFMT_bits_ptr->DATDLY       = 0x1; //delay in bits
    //Set TX data mask
    * MCASP1_TXMASK = 0x0000ffff; /* Last 16 bit matter */
    //Active slots
    * MCASP1_TXTDM_ptr                  = 0x0;
    MCASP1_TXTDM_bits_ptr->TDMS0        = 0x1;
    MCASP1_TXTDM_bits_ptr->TDMS1        = 0x1;
    //Setup serializers
    MCASP1_SRCTL_TX_bits_ptr->SRMOD     = 0x1; //transmitter
    MCASP1_SRCTL_TX_bits_ptr->DISMOD    = 0x0; //tri-state
#endif
}
/*--- End of the function ----------------------------------------------------*/

static inline void setup_mcasp1_RX (void)
{
#if (MCASP1_RX)
start:
    //Reset RX
    * MCASP1_GBLCTLR_ptr = 0x0;
    WAIT_REG_RESET(* MCASP1_GBLCTLR_ptr);

    //Switch off RX serializer
    * MCASP1_SRCTL_RX_ptr                 = 0x0;

    //Switch off all RX event generation
    * MCASP1_IRQCTLR                    = 0x0; //All IRQs are disabled
    MCASP1_DMACTLR_bits_ptr->DATDMA     = 0x1; //Disable DMA requests generation

    //Clear RX stat
    * MCASP1_RXSTAT_ptr                 = 0xffffffff;

    //RX high speed clock
    //Cause MCASP1_ACLKXCTL_bits_ptr->ASYNC == 0x0
    MCASP1_AHCLKRCTL_bits_ptr->HCLKDIV  = 4095;
    MCASP1_AHCLKRCTL_bits_ptr->HCLKP    = 0x0; //0 == Rising 1 == Falling
    MCASP1_AHCLKRCTL_bits_ptr->HCLKM    = 0x0; //1 == internal 0 == external

    //RX clock setup
    MCASP1_ACLKRCTL_bits_ptr->CLKDIV    = 0x0;
    MCASP1_ACLKRCTL_bits_ptr->CLKM      = 0x0; //1 == internal 0 == external
    MCASP1_ACLKRCTL_bits_ptr->CLKP      = 0x0; //0 == Rising 1 == Falling

    //RX clock control
    MCASP1_RXCLKCHK_bits_ptr->MIN       = 0x0;
    MCASP1_RXCLKCHK_bits_ptr->MAX       = 0xff;
    MCASP1_RXCLKCHK_bits_ptr->PS        = 0x8; //McASP interface clock divided by 256

    //RX frame format
    MCASP1_RXFMCTL_bits_ptr->FSP        = 0x1; //start of the frame : 1 == falling edge 0 == rising edge
    MCASP1_RXFMCTL_bits_ptr->FSM        = 0x1; //Internally
    MCASP1_RXFMCTL_bits_ptr->FWID       = 0x1; //Word wide frame
    MCASP1_RXFMCTL_bits_ptr->MOD        = 0x2; //I2S mode

    //RX data format
    MCASP1_RXFMT_bits_ptr->ROT          = 0x0; //0x0 == no rotation 0x4 == 16 bit rotation
    MCASP1_RXFMT_bits_ptr->BUSEL        = 0x1; //1 == CFG bus
    MCASP1_RXFMT_bits_ptr->SSZ          = 0xf; //32 bit per slot
    MCASP1_RXFMT_bits_ptr->RVRS         = 0x1; //1 == MSB first
    MCASP1_RXFMT_bits_ptr->DATDLY       = 0x1; //delay in bits

    //Set data mask
    * MCASP1_RXMASK = 0x0000ffff; /* Last 16 bit matter */

    //Active slots
    * MCASP1_RXTDM_ptr                  = 0x0;
    MCASP1_RXTDM_bits_ptr->TDMS0        = 0x1;
    MCASP1_RXTDM_bits_ptr->TDMS1        = 0x1;

    //Setup serializers
    MCASP1_SRCTL_RX_bits_ptr->SRMOD     = 0x2; //receiver
    MCASP1_SRCTL_RX_bits_ptr->DISMOD    = 0x0; //tri-state
#endif
}
/*--- End of the function ----------------------------------------------------*/

int32_t start_mcasp1_TX (void)
{
#if (MCASP1_TX)
    uint32_t timeout_counter = 0;

    /** Start TX high-speed clock */
    MCASP1_GBLCTLX_bits_ptr->XHCLKRST = 0x1;
    while (!MCASP1_GBLCTLX_bits_ptr->XHCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_AM5728)
            set_status_0_red();
#endif
            return -1;
        }
    };

    /** Start TX clock */
    MCASP1_GBLCTLX_bits_ptr->XCLKRST = 0x1;
    timeout_counter = 0;
    while (!MCASP1_GBLCTLX_bits_ptr->XCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_AM5728)
            set_status_0_red();
#endif
            return -1;
        }
    };

    //Clear stat
    * MCASP1_TXSTAT_ptr = 0xffffffff;

    /** Flush TX buffers to an empty state */
    MCASP1_GBLCTLX_bits_ptr->XSRCLR = 0x1;
    timeout_counter = 0;
    while (!MCASP1_GBLCTLX_bits_ptr->XSRCLR)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_AM5728)
            set_status_0_red();
#endif
            return -1;
        }
    };

    MCASP1_TXSTAT_bits_ptr->DATA = 0x1;

    /** Start TX state machine */
    MCASP1_GBLCTLX_bits_ptr->XSMRST = 0x1;
    timeout_counter = 0;
    while (!MCASP1_GBLCTLX_bits_ptr->XSMRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_AM5728)
            set_status_0_red();
#endif
            return -1;
        }
    };

    //Write to the TX buffer to prevent underflow
    * MCASP1_TXBUF_ptr = 0x0;
    * MCASP1_TXBUF_ptr = 0x0;

    /** Start TX frame sync generator */
    MCASP1_GBLCTLX_bits_ptr->XFRST = 0x1;
    timeout_counter = 0;
    while (!MCASP1_GBLCTLX_bits_ptr->XFRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_AM5728)
            set_status_0_red();
#endif
            return -1;
        }
    };
#endif
    return 0;
}
/*--- End of the function ----------------------------------------------------*/

static int32_t start_mcasp1_RX (void)
{
#if (MCASP1_RX)
    uint32_t timeout_counter = 0;

    /** Start RX high-speed clock */
    MCASP1_GBLCTLR_bits_ptr->RHCLKRST = 0x1;
    while (!MCASP1_GBLCTLR_bits_ptr->RHCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_AM5728)
            set_status_1_red();
#endif
            return -1;
        }
    };

    /** Start RX clock */
    MCASP1_GBLCTLR_bits_ptr->RCLKRST = 0x1;
    timeout_counter = 0;
    while (!MCASP1_GBLCTLR_bits_ptr->RCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_AM5728)
            set_status_1_red();
#endif
            return -1;
        }
    };

    //Clear stat
    * MCASP1_RXSTAT_ptr = 0xffffffff;

    /** Flush RX buffers to an empty state */
    MCASP1_GBLCTLR_bits_ptr->RSRCLR = 0x1;
    timeout_counter = 0;
    while (!MCASP1_GBLCTLR_bits_ptr->RSRCLR)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_AM5728)
            set_status_1_red();
#endif
            return -1;
        }
    };

    MCASP1_RXSTAT_bits_ptr->DATA = 0x1;

    /** Start RX state machine */
    MCASP1_GBLCTLR_bits_ptr->RSMRST = 0x1;
    timeout_counter = 0;
    while (!MCASP1_GBLCTLR_bits_ptr->RSMRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_AM5728)
            set_status_1_red();
#endif
            return -1;
        }
    };

    //Read the RX buffer to prevent underflow
    uint32_t    dump = * MCASP1_RXBUF_ptr;
                dump = * MCASP1_RXBUF_ptr;

    /** Start frame sync generator */
    MCASP1_GBLCTLR_bits_ptr->RFRST = 0x1;
    timeout_counter = 0;
    while (!MCASP1_GBLCTLR_bits_ptr->RFRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_AM5728)
            set_status_1_red();
#endif
            return -1;
        }
    };
#endif
    return 0;
}
/*--- End of the function ----------------------------------------------------*/


void init_mcasp1 (void)
{
#if (MCASP1_TX||MCASP1_RX)
    uint32_t pid1 = 0;
    pid1 = *(uint32_t *)MCASP1_CFG_ADDR;
start:
    * MCASP1_GBLCTL_ptr = 0;
    WAIT_REG_RESET(* MCASP1_GBLCTL_ptr);

    //Switch off all the serializes
    * MCASP1_SRCTL_TX_ptr     = 0x0;
    * MCASP1_SRCTL_RX_ptr     = 0x0;
    * MCASP1_XRSRCTL_00_ptr   = 0x0;
    * MCASP1_XRSRCTL_01_ptr   = 0x0;
    * MCASP1_XRSRCTL_02_ptr   = 0x0;
    * MCASP1_XRSRCTL_03_ptr   = 0x0;
    * MCASP1_XRSRCTL_04_ptr   = 0x0;
    * MCASP1_XRSRCTL_05_ptr   = 0x0;
    * MCASP1_XRSRCTL_06_ptr   = 0x0;
    * MCASP1_XRSRCTL_07_ptr   = 0x0;

    * MCASP1_XRSRCTL_09_ptr   = 0x0;
    * MCASP1_XRSRCTL_10_ptr   = 0x0;
    * MCASP1_XRSRCTL_11_ptr   = 0x0;
    * MCASP1_XRSRCTL_12_ptr   = 0x0;
    * MCASP1_XRSRCTL_13_ptr   = 0x0;

    * MCASP1_XRSRCTL_15_ptr   = 0x0;

    /** Global McASP1 setup */
    * MCASP1_LBCTL_ptr        = 0x0; //No loop-back
    * MCASP1_TXDITCTL_ptr     = 0x0; //No DIT

    setup_mcasp1_TX();
    setup_mcasp1_RX();

    setup_mcasp1_pads();

    if (start_mcasp1_TX() < 0) goto start;
    if (start_mcasp1_RX() < 0) goto start;

#endif
}
/*--- End of the function ----------------------------------------------------*/

#endif
