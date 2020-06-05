#include "mcasp_init.h"

/*!
 * @file
 * In this file McASP initialization routines provided.
 */

#if (!OPTIMIZED)
struct MCASP_DMACTL *       MCASP_DMACTLX_bits_ptr     = (struct MCASP_DMACTL *)   (MCASP_CFG_ADDR + MCASP_DMACTLX_OFF);
struct MCASP_DMACTL *       MCASP_DMACTLR_bits_ptr     = (struct MCASP_DMACTL *)   (MCASP_CFG_ADDR + MCASP_DMACTLR_OFF);
struct MCASP_PFUNC *        MCASP_PFUNC_bits_ptr       = (struct MCASP_PFUNC *)    (MCASP_CFG_ADDR + MCASP_PFUNC_OFF);
struct MCASP_PDIR *         MCASP_PDIR_bits_ptr        = (struct MCASP_PDIR *)     (MCASP_CFG_ADDR + MCASP_PDIR_OFF);
struct MCASP_AHCLKCTL *     MCASP_AHCLKXCTL_bits_ptr   = (struct MCASP_AHCLKCTL *) (MCASP_CFG_ADDR + MCASP_AHCLKXCTL_OFF);
struct MCASP_AHCLKCTL *     MCASP_AHCLKRCTL_bits_ptr   = (struct MCASP_AHCLKCTL *) (MCASP_CFG_ADDR + MCASP_AHCLKRCTL_OFF);
struct MCASP_ACLKCTL *      MCASP_ACLKXCTL_bits_ptr    = (struct MCASP_ACLKCTL *)  (MCASP_CFG_ADDR + MCASP_ACLKXCTL_OFF);
struct MCASP_ACLKCTL *      MCASP_ACLKRCTL_bits_ptr    = (struct MCASP_ACLKCTL *)  (MCASP_CFG_ADDR + MCASP_ACLKRCTL_OFF);
struct MCASP_CLKCHK *       MCASP_TXCLKCHK_bits_ptr    = (struct MCASP_CLKCHK *)   (MCASP_CFG_ADDR + MCASP_TXCLKCHK_OFF);
struct MCASP_CLKCHK *       MCASP_RXCLKCHK_bits_ptr    = (struct MCASP_CLKCHK *)   (MCASP_CFG_ADDR + MCASP_RXCLKCHK_OFF);
struct MCASP_FMCTL *        MCASP_TXFMCTL_bits_ptr     = (struct MCASP_FMCTL *)    (MCASP_CFG_ADDR + MCASP_TXFMCTL_OFF);
struct MCASP_FMCTL *        MCASP_RXFMCTL_bits_ptr     = (struct MCASP_FMCTL *)    (MCASP_CFG_ADDR + MCASP_RXFMCTL_OFF);
struct MCASP_XFMT *         MCASP_TXFMT_bits_ptr       = (struct MCASP_XFMT *)     (MCASP_CFG_ADDR + MCASP_TXFMT_OFF);
struct MCASP_XFMT *         MCASP_RXFMT_bits_ptr       = (struct MCASP_XFMT *)     (MCASP_CFG_ADDR + MCASP_RXFMT_OFF);
struct MCASP_TDMS *         MCASP_TXTDM_bits_ptr       = (struct MCASP_TDMS *)     (MCASP_CFG_ADDR + MCASP_TXTDM_OFF);
struct MCASP_TDMS *         MCASP_RXTDM_bits_ptr       = (struct MCASP_TDMS *)     (MCASP_CFG_ADDR + MCASP_RXTDM_OFF);
#if     (MCASP_CFG_ADDR == MCASP1_CFG_ADDR)
struct MCASP_SRCTL *        MCASP_SRCTL_TX_bits_ptr    = (struct MCASP_SRCTL *)    (MCASP_CFG_ADDR + MCASP_XRSRCTL_08_OFF);
struct MCASP_SRCTL *        MCASP_SRCTL_RX_bits_ptr    = (struct MCASP_SRCTL *)    (MCASP_CFG_ADDR + MCASP_XRSRCTL_14_OFF);
#elif   (MCASP_CFG_ADDR == MCASP8_CFG_ADDR)
struct MCASP_SRCTL *        MCASP_SRCTL_TX_bits_ptr    = (struct MCASP_SRCTL *)    (MCASP_CFG_ADDR + MCASP_XRSRCTL_00_OFF);
struct MCASP_SRCTL *        MCASP_SRCTL_RX_bits_ptr    = (struct MCASP_SRCTL *)    (MCASP_CFG_ADDR + MCASP_XRSRCTL_01_OFF);
#endif
#endif

/* McASP common setup */
struct MCASP_GBLCTL *       MCASP_GBLCTL_bits_ptr      = (struct MCASP_GBLCTL *)   (MCASP_CFG_ADDR + MCASP_GBLCTL_OFF);
uint32_t *                  MCASP_GBLCTL_ptr           = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_GBLCTL_OFF);

/* When writing to this register, only the TRANSMIT bits of GBLCTL are affected */
struct MCASP_GBLCTL *       MCASP_GBLCTLX_bits_ptr     = (struct MCASP_GBLCTL *)   (MCASP_CFG_ADDR + MCASP_GBLCTLX_OFF);
uint32_t *                  MCASP_GBLCTLX_ptr          = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_GBLCTLX_OFF);

/* When writing to this register, only the RECEIVE bits of GBLCTL are affected */
struct MCASP_GBLCTL *       MCASP_GBLCTLR_bits_ptr     = (struct MCASP_GBLCTL *)   (MCASP_CFG_ADDR + MCASP_GBLCTLR_OFF);
uint32_t *                  MCASP_GBLCTLR_ptr          = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_GBLCTLR_OFF);

uint32_t *                  MCASP_LBCTL_ptr            = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_LBCTL_OFF);
uint32_t *                  MCASP_TXDITCTL_ptr         = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_TXDITCTL_OFF);
uint32_t *                  MCASP_IRQCTLX              = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_IRQCTLX_OFF);
uint32_t *                  MCASP_IRQCTLR              = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_IRQCTLR_OFF);
uint32_t *                  MCASP_DMACTLX_ptr          = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_DMACTLX_OFF);
uint32_t *                  MCASP_DMACTLR_ptr          = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_DMACTLR_OFF);
/* McASP pad setup */
uint32_t *                  MCASP_PFUNC_ptr            = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_PFUNC_OFF);
uint32_t *                  MCASP_PDIR_ptr             = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_PDIR_OFF);
/* McASP xmission setup */
uint32_t *                  MCASP_TXMASK               = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_TXMASK_OFF);
uint32_t *                  MCASP_RXMASK               = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_RXMASK_OFF);
#if     ((MCASP_CFG_ADDR == MCASP1_CFG_ADDR)&&(IDK_AM5728))
    #if (CRISS_CROSS)
        uint32_t * MCASP_RXBUF_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_RXBUF_08_OFF);
        uint32_t * MCASP_TXBUF_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_TXBUF_14_OFF);
    #else
        uint32_t * MCASP_TXBUF_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_TXBUF_08_OFF);
        uint32_t * MCASP_RXBUF_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_RXBUF_14_OFF);
    #endif
#elif   ((MCASP_CFG_ADDR == MCASP8_CFG_ADDR)||(BBB_AM335x))
    uint32_t * MCASP_TXBUF_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_TXBUF_00_OFF);
    uint32_t * MCASP_RXBUF_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_RXBUF_01_OFF);
#endif
/* McASP clock setup */
uint32_t *                  MCASP_AHCLKXCTL_ptr        = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_AHCLKXCTL_OFF);
uint32_t *                  MCASP_AHCLKRCTL_ptr        = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_AHCLKRCTL_OFF);
uint32_t *                  MCASP_ACLKXCTL_ptr         = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_ACLKXCTL_OFF);
uint32_t *                  MCASP_ACLKRCTL_ptr         = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_ACLKRCTL_OFF);
uint32_t *                  MCASP_TXCLKCHK_ptr         = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_TXCLKCHK_OFF);
uint32_t *                  MCASP_RXCLKCHK_ptr         = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_RXCLKCHK_OFF);

/* McASP frame setup */
uint32_t *                  MCASP_TXFMCTL_ptr          = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_TXFMCTL_OFF);
uint32_t *                  MCASP_RXFMCTL_ptr          = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_RXFMCTL_OFF);

uint32_t *                  MCASP_TXFMT_ptr            = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_TXFMT_OFF);
uint32_t *                  MCASP_RXFMT_ptr            = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_RXFMT_OFF);

uint32_t *                  MCASP_TXTDM_ptr            = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_TXTDM_OFF);
uint32_t *                  MCASP_RXTDM_ptr            = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_RXTDM_OFF);

/* McASP flags */
uint32_t *                  MCASP_TXSTAT_ptr           = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_TXSTAT_OFF);
uint32_t *                  MCASP_RXSTAT_ptr           = (uint32_t *)              (MCASP_CFG_ADDR + MCASP_RXSTAT_OFF);

struct MCASP_STAT *         MCASP_TXSTAT_bits_ptr      = (struct MCASP_STAT *)     (MCASP_CFG_ADDR + MCASP_TXSTAT_OFF);
struct MCASP_STAT *         MCASP_RXSTAT_bits_ptr      = (struct MCASP_STAT *)     (MCASP_CFG_ADDR + MCASP_RXSTAT_OFF);

struct MCASP_TDMSLOT *      MCASP_TXTDMSLOT_bits_ptr   = (struct MCASP_TDMSLOT *)  (MCASP_CFG_ADDR + MCASP_TXTDMSLOT_OFF);
struct MCASP_TDMSLOT *      MCASP_RXTDMSLOT_bits_ptr   = (struct MCASP_TDMSLOT *)  (MCASP_CFG_ADDR + MCASP_RXTDMSLOT_OFF);

/* McASP serializers control */
#if     ((MCASP_CFG_ADDR == MCASP1_CFG_ADDR)&&(IDK_AM5728))
    uint32_t * MCASP_XRSRCTL_00_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_00_OFF);
    uint32_t * MCASP_XRSRCTL_01_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_01_OFF);
    uint32_t * MCASP_XRSRCTL_02_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_02_OFF);
    uint32_t * MCASP_XRSRCTL_03_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_03_OFF);
    uint32_t * MCASP_XRSRCTL_04_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_04_OFF);
    uint32_t * MCASP_XRSRCTL_05_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_05_OFF);
    uint32_t * MCASP_XRSRCTL_06_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_06_OFF);
    uint32_t * MCASP_XRSRCTL_07_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_07_OFF);
    #if (CRISS_CROSS)
        uint32_t * MCASP_SRCTL_RX_ptr   = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_08_OFF); /* ! */
    #else
        uint32_t * MCASP_SRCTL_TX_ptr   = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_08_OFF); /* ! */
    #endif
    uint32_t * MCASP_XRSRCTL_09_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_09_OFF);
    uint32_t * MCASP_XRSRCTL_10_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_10_OFF);
    uint32_t * MCASP_XRSRCTL_11_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_11_OFF);
    uint32_t * MCASP_XRSRCTL_12_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_12_OFF);
    uint32_t * MCASP_XRSRCTL_13_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_13_OFF);
    #if (CRISS_CROSS)
        uint32_t * MCASP_SRCTL_TX_ptr   = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_14_OFF); /* ! */
    #else
        uint32_t * MCASP_SRCTL_RX_ptr   = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_14_OFF); /* ! */
    #endif
    uint32_t * MCASP_XRSRCTL_15_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_15_OFF);
#elif   ((MCASP_CFG_ADDR == MCASP8_CFG_ADDR)||(BBB_AM335x))
    uint32_t * MCASP_SRCTL_TX_ptr   = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_00_OFF); /* ! */
    uint32_t * MCASP_SRCTL_RX_ptr   = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_01_OFF); /* ! */
    uint32_t * MCASP_XRSRCTL_02_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_02_OFF);
    uint32_t * MCASP_XRSRCTL_03_ptr = (uint32_t *) (MCASP_CFG_ADDR + MCASP_XRSRCTL_03_OFF);
#endif

/*
 * This setup is actual for IDK5728.
 * AXR0 should be TX, and AXR1 should be RX, for the production design
 */
#define BIT(x)                      (uint32_t)(1<<x)
#define AXR0_BIT                    BIT(0)
#define AXR1_BIT                    BIT(1)
#define AXR8_BIT                    BIT(8)
#define AXR14_BIT                   BIT(14)
#define ACLKX_BIT                   BIT(26)
#define AFSX_BIT                    BIT(28)

#if     ((MCASP_CFG_ADDR == MCASP1_CFG_ADDR)&&(IDK_AM5728))
#define MCASP_PFUNC_SETUP           (uint32_t)( ~(  AXR8_BIT | AXR14_BIT | ACLKX_BIT | AFSX_BIT ) )
    #if (CDC_IS_MASTER)
        #if (CRISS_CROSS)
            #define MCASP_PDIR_SETUP    (uint32_t)(     0x0      | AXR14_BIT | 0x0       | 0x0        ) /* All in except 14. */
        #else
            #define MCASP_PDIR_SETUP    (uint32_t)(     AXR8_BIT | 0x0       | 0x0       | 0x0        )
        #endif
    #else
        #if (CRISS_CROSS)
            #define MCASP_PDIR_SETUP    (uint32_t)(     0x0      | AXR14_BIT | ACLKX_BIT | AFSX_BIT   )
        #else
            #define MCASP_PDIR_SETUP    (uint32_t)(     AXR8_BIT | 0x0       | ACLKX_BIT | AFSX_BIT   )
        #endif
    #endif
#elif   ((MCASP_CFG_ADDR == MCASP8_CFG_ADDR)||(BBB_AM335x))
#define MCASP_PFUNC_SETUP           (uint32_t)( ~(  AXR0_BIT | AXR1_BIT  | ACLKX_BIT | AFSX_BIT ) )
    #if (CDC_IS_MASTER)
        #define MCASP_PDIR_SETUP    (uint32_t)(     AXR0_BIT | 0x0       | 0x0       | 0x0        )
    #else
        #define MCASP_PDIR_SETUP    (uint32_t)(     AXR0_BIT | 0x0       | ACLKX_BIT | AFSX_BIT   )
    #endif
#endif

#define AHCLK_CTL_HCLKDIV(x)        (uint32_t)( x - 1 ) /* 0x0: Divide-by-1; 0x1: Divide-by-2; ... */
#define AHCLK_CTL_HCLKP             BIT(14)             /* 0x0: Falling edge; 0x1: Rising edge. */
#define AHCLK_CTL_HCLKM             BIT(15)             /* 0x0: External; 0x1: Internal. */

#define ACLK_CTL_CLKDIV(x)          (uint32_t)( x - 1 ) /* 0x0: Divide-by-1; 0x1: Divide-by-2; ... */
#define ACLK_CTL_CLKM               BIT(5)              /* 0x0: External; 0x1: Internal. */
#define ACLK_CTL_CLKP               BIT(7)              /* 0x0: Falling edge; 0x1: Rising edge. */

#if (IDK_AM5728)
    #if CDC_IS_MASTER                            /* Divide                  | Polarity        | Ext/Int */
        #define AHCLKX_CTL_SETUP        (uint32_t)( AHCLK_CTL_HCLKDIV(10)   | 0x0             | 0x0             ) /* ASYNC = 0 -> Synchronous */
        #define AHCLKR_CTL_SETUP        (uint32_t)( AHCLK_CTL_HCLKDIV(10)   | 0x0             | 0x0             )
    #else
        #define AHCLKX_CTL_SETUP        (uint32_t)( AHCLK_CTL_HCLKDIV(10)   | AHCLK_CTL_HCLKP | AHCLK_CTL_HCLKM )
        #define AHCLKR_CTL_SETUP        (uint32_t)( AHCLK_CTL_HCLKDIV(10)   | 0x0             | 0x0             )
    #endif /* CDC_IS_MASTER */
    #if CDC_IS_MASTER                            /* Divide                  | Polarity        | Ext/Int */
        #define ACLKX_CTL_SETUP         (uint32_t)( ACLK_CTL_CLKDIV(10)     | ACLK_CTL_CLKP   | 0x0             )
        #define ACLKR_CTL_SETUP         (uint32_t)( ACLK_CTL_CLKDIV(10)     | ACLK_CTL_CLKP   | 0x0             )
    #else
        #define ACLKX_CTL_SETUP         (uint32_t)( ACLK_CTL_CLKDIV(10)     | ACLK_CTL_CLKP   | ACLK_CTL_CLKM   )
        #define ACLKR_CTL_SETUP         (uint32_t)( ACLK_CTL_CLKDIV(10)     | 0x0             | 0x0             )
    #endif /* CDC_IS_MASTER */
#elif (BBB_AM335x) /* McASP input freq. 24MHz. */
    #if CDC_IS_MASTER                            /* Divide                  | Polarity        | Ext/Int */
        #define AHCLKX_CTL_SETUP        (uint32_t)( AHCLK_CTL_HCLKDIV(24)   | 0x0             | 0x0             ) /* ASYNC = 0 -> Synchronous */
        #define AHCLKR_CTL_SETUP        (uint32_t)( AHCLK_CTL_HCLKDIV(24)   | 0x0             | 0x0             )
    #else                                        /* To 1MHz */
        #define AHCLKX_CTL_SETUP        (uint32_t)( AHCLK_CTL_HCLKDIV(24)   | AHCLK_CTL_HCLKP | AHCLK_CTL_HCLKM )
        #define AHCLKR_CTL_SETUP        (uint32_t)( AHCLK_CTL_HCLKDIV(24)   | 0x0             | 0x0             )
    #endif /* CDC_IS_MASTER */
    #if CDC_IS_MASTER                            /* Divide                  | Polarity        | Ext/Int */
        #define ACLKX_CTL_SETUP         (uint32_t)( ACLK_CTL_CLKDIV(1)      | ACLK_CTL_CLKP   | 0x0             )
        #define ACLKR_CTL_SETUP         (uint32_t)( ACLK_CTL_CLKDIV(1)      | ACLK_CTL_CLKP   | 0x0             )
    #else
        #define ACLKX_CTL_SETUP         (uint32_t)( ACLK_CTL_CLKDIV(1)      | ACLK_CTL_CLKP   | ACLK_CTL_CLKM   )
        #define ACLKR_CTL_SETUP         (uint32_t)( ACLK_CTL_CLKDIV(1)      | 0x0             | 0x0             )
    #endif /* CDC_IS_MASTER */
#endif /* BBB_AM335x */

/* Clock check prescaler value. */
#define RPS                         0x8 /* 0x8: McASP interface clock divided by 256 */
#define XPS                         0x8 /* 0x8: McASP interface clock divided by 256 */
#define CLK_CHK_MIN(x)              (uint32_t)( x << 8 )
#define CLK_CHK_MAX(x)              (uint32_t)( x << 16 )
#define TX_CLK_CHK_SETUP            (uint32_t)( XPS | CLK_CHK_MIN(0x0) | CLK_CHK_MAX(0xff) )
#define RX_CLK_CHK_SETUP            (uint32_t)( RPS | CLK_CHK_MIN(0x0) | CLK_CHK_MAX(0xff) )

#define FM_CTL_FSP                  BIT(0)
#define FM_CTL_FSM                  BIT(1)
#define FM_CTL_FWID                 BIT(4)
#define FM_CTL_MOD(x)               (uint32_t)( x << 7 )
#if CDC_IS_MASTER                            /* Polarity   | Ext/Int    | Width       | Mode */
    #define TX_FM_CTL_SETUP         (uint32_t)( FM_CTL_FSP | 0x0        | FM_CTL_FWID | FM_CTL_MOD(0x2) )
    #define RX_FM_CTL_SETUP         (uint32_t)( FM_CTL_FSP | 0x0        | FM_CTL_FWID | FM_CTL_MOD(0x2) )
#else
    #define TX_FM_CTL_SETUP         (uint32_t)( FM_CTL_FSP | FM_CTL_FSM | FM_CTL_FWID | FM_CTL_MOD(0x2) )
    #define RX_FM_CTL_SETUP         (uint32_t)( FM_CTL_FSP | FM_CTL_FSM | FM_CTL_FWID | FM_CTL_MOD(0x2) )
#endif
#define FMT_XROT(x)                 (uint32_t)( x / 4 )
#define FMT_BUSSEL                  BIT(3)
#define FMT_SSZ(x)                  (uint32_t)( x << 4 )
#define FMT_RVRS                    BIT(15)
#define FMT_DAT_DLY(x)              (uint32_t)( x << 16 )
#if CDC_IS_MASTER                            /* Rotation     | Bus        | Slot size    | Order    | Delay */
    #define TX_FMT_SETUP            (uint32_t)( FMT_XROT(16) | FMT_BUSSEL | FMT_SSZ(0xf) | FMT_RVRS | FMT_DAT_DLY(0x1) )
    #define RX_FMT_SETUP            (uint32_t)( FMT_XROT(16) | FMT_BUSSEL | FMT_SSZ(0xf) | FMT_RVRS | FMT_DAT_DLY(0x1) )
#else
    #define TX_FMT_SETUP            (uint32_t)( FMT_XROT(16) | FMT_BUSSEL | FMT_SSZ(0xf) | FMT_RVRS | FMT_DAT_DLY(0x1) )
    #define RX_FMT_SETUP            (uint32_t)( FMT_XROT(16) | FMT_BUSSEL | FMT_SSZ(0xf) | FMT_RVRS | FMT_DAT_DLY(0x1) )
#endif

/* This function should take into account all McASPs present in the program */
static inline void service_buffs (void)
{
    uint32_t dump = 0;
#if (MCASP_RX)
    if (MCASP_RXSTAT_bits_ptr->DATA) //service all RX buffs
    {
        dump = * MCASP_RXBUF_ptr;
    }
#endif
#if (MCASP_TX)
    if (MCASP_TXSTAT_bits_ptr->DATA) //service all TX bufs
    {
        * MCASP_TXBUF_ptr =dump;
    }
#endif
}

static inline void setup_mcasp_pads (void)
{
#if(!OPTIMIZED)
    #if     (MCASP_CFG_ADDR == MCASP1_CFG_ADDR)
        * MCASP_PFUNC_ptr = 0xffffffff;
          MCASP_PFUNC_bits_ptr->AXR8          = 0x0;
          MCASP_PFUNC_bits_ptr->AXR14         = 0x0;
          MCASP_PFUNC_bits_ptr->ACLKX         = 0x0;
          MCASP_PFUNC_bits_ptr->AFSX          = 0x0;
          //Set pin direction
          MCASP_PDIR_bits_ptr->AXR8           = 0x1; //out
          MCASP_PDIR_bits_ptr->AXR14          = 0x0; //in
          MCASP_PDIR_bits_ptr->ACLKX          = 0x1; //out
          MCASP_PDIR_bits_ptr->AFSX           = 0x1; //out
    #elif   (MCASP_CFG_ADDR == MCASP8_CFG_ADDR)
        * MCASP_PFUNC_ptr = 0xffffffff;
          MCASP_PFUNC_bits_ptr->AXR0        = 0x0;
          MCASP_PFUNC_bits_ptr->AXR1        = 0x0;
          MCASP_PFUNC_bits_ptr->ACLKX       = 0x0;
          MCASP_PFUNC_bits_ptr->AFSX        = 0x0;
          //Set pin direction
          MCASP_PDIR_bits_ptr->AXR0         = 0x1; //out
          MCASP_PDIR_bits_ptr->AXR1         = 0x0; //in
          MCASP_PDIR_bits_ptr->ACLKX        = 0x1; //out
          MCASP_PDIR_bits_ptr->AFSX         = 0x1; //out
    #endif
#else
#if (1)
          * MCASP_PFUNC_ptr  = MCASP_PFUNC_SETUP;
          * MCASP_PDIR_ptr   = MCASP_PDIR_SETUP;
#else
          * MCASP_PFUNC_ptr  = 0x0; /* All McASP. */
          * MCASP_PDIR_ptr   = 0xffffffff; /* All out. */
#endif
#endif
}
#if(!OPTIMIZED)
static inline void setup_mcasp_TX (void)
{
start:
    //Reset TX part
    * MCASP_GBLCTLX_ptr = 0x0;
    WAIT_REG_RESET(* MCASP_GBLCTLX_ptr);
    //Switch off TX serializer
    * MCASP_SRCTL_TX_ptr               = 0x0;
    //Switch off all TX event generation
    * MCASP_IRQCTLX                    = 0x0; //All IRQs are disabled
    MCASP_DMACTLX_bits_ptr->DATDMA     = 0x1; //Disable DMA requests generation
    //Clear TX stat
    * MCASP_TXSTAT_ptr                 = 0xffffffff;
    //TX high speed clock
    MCASP_AHCLKXCTL_bits_ptr->HCLKDIV  = 100-1; /* PER_ABE_X1_GFCLK -> MCASP_AUX_GFCLK = 100 MHz */
    MCASP_AHCLKXCTL_bits_ptr->HCLKP    = 0x1; //0 == Rising 1 == Falling
    MCASP_AHCLKXCTL_bits_ptr->HCLKM    = 0x1; //Internal AHCLKX
    //TX clock setup
    MCASP_ACLKXCTL_bits_ptr->CLKDIV    = 0x0;
    MCASP_ACLKXCTL_bits_ptr->CLKM      = 0x1; //Internal (from divider)
    MCASP_ACLKXCTL_bits_ptr->ASYNC     = 0x0; //Synchronous
    MCASP_ACLKXCTL_bits_ptr->CLKP      = 0x1; //0 == Rising 1 == Falling
    //TX clock control
    MCASP_TXCLKCHK_bits_ptr->MIN       = 0x0;
    MCASP_TXCLKCHK_bits_ptr->MAX       = 0xff;
    MCASP_TXCLKCHK_bits_ptr->PS        = 0x8; //McASP interface clock divided by 256
    //TX frame format
    MCASP_TXFMCTL_bits_ptr->FSP        = 0x1; //start of the frame : 1 == falling edge 0 == rising edge
    MCASP_TXFMCTL_bits_ptr->FSM        = 0x1; //Internally
    MCASP_TXFMCTL_bits_ptr->FWID       = 0x1; //Word wide frame
    MCASP_TXFMCTL_bits_ptr->MOD        = 0x2; //I2S mode
    //TX data format
    MCASP_TXFMT_bits_ptr->ROT          = 0x4; //0x4 == 16 bit rotation
    MCASP_TXFMT_bits_ptr->BUSEL        = 0x1; //1 == CFG bus
    MCASP_TXFMT_bits_ptr->SSZ          = 0xf; //32 bit per slot
    MCASP_TXFMT_bits_ptr->RVRS         = 0x1; //1 == MSB first
    MCASP_TXFMT_bits_ptr->DATDLY       = 0x1; //delay in bits
    //Set TX data mask
    * MCASP_TXMASK = 0x0000ffff; /* Last 16 bit matter */
    //Active slots
    * MCASP_TXTDM_ptr                  = 0x0;
    MCASP_TXTDM_bits_ptr->TDMS0        = 0x1;
    MCASP_TXTDM_bits_ptr->TDMS1        = 0x1;
    //Setup serializers
    MCASP_SRCTL_TX_bits_ptr->SRMOD     = 0x1; //transmitter
    MCASP_SRCTL_TX_bits_ptr->DISMOD    = 0x0; //tri-state
}
static inline void setup_mcasp_RX (void)
{
start:
    //Reset RX
    * MCASP_GBLCTLR_ptr = 0x0;
    WAIT_REG_RESET(* MCASP_GBLCTLR_ptr);
    //Switch off RX serializer
    * MCASP_SRCTL_RX_ptr               = 0x0;
    //Switch off all RX event generation
    * MCASP_IRQCTLR                    = 0x0;   //All IRQs are disabled
    MCASP_DMACTLR_bits_ptr->DATDMA     = 0x1;   //Disable DMA requests generation
    //Clear RX stat
    * MCASP_RXSTAT_ptr                 = 0xffffffff;
    //RX high speed clock
    MCASP_AHCLKRCTL_bits_ptr->HCLKDIV  = 4095;  //Cause MCASP_ACLKXCTL_bits_ptr->ASYNC == 0x0
    MCASP_AHCLKRCTL_bits_ptr->HCLKP    = 0x0;   //0 == Rising 1 == Falling
    MCASP_AHCLKRCTL_bits_ptr->HCLKM    = 0x0;   //1 == internal 0 == external
    //RX clock setup
    MCASP_ACLKRCTL_bits_ptr->CLKDIV    = 0x0;
    MCASP_ACLKRCTL_bits_ptr->CLKM      = 0x0;   //1 == internal 0 == external
    MCASP_ACLKRCTL_bits_ptr->CLKP      = 0x0;   //0 == Rising 1 == Falling
    //RX clock control
    MCASP_RXCLKCHK_bits_ptr->MIN       = 0x0;
    MCASP_RXCLKCHK_bits_ptr->MAX       = 0xff;
    MCASP_RXCLKCHK_bits_ptr->PS        = 0x8;   //McASP interface clock divided by 256
    //RX frame format
    MCASP_RXFMCTL_bits_ptr->FSP        = 0x1;   //start of the frame : 1 == falling edge 0 == rising edge
    MCASP_RXFMCTL_bits_ptr->FSM        = 0x1;   //Internally
    MCASP_RXFMCTL_bits_ptr->FWID       = 0x1;   //Word wide frame
    MCASP_RXFMCTL_bits_ptr->MOD        = 0x2;   //I2S mode
    //RX data format
    MCASP_RXFMT_bits_ptr->ROT         = 0x4;   //0x0 == no rotation 0x4 == 16 bit rotation
    MCASP_RXFMT_bits_ptr->BUSEL       = 0x1;   //1 == CFG bus
    MCASP_RXFMT_bits_ptr->SSZ         = 0xf;   //32 bit per slot
    MCASP_RXFMT_bits_ptr->RVRS        = 0x1;   //1 == MSB first
    MCASP_RXFMT_bits_ptr->DATDLY      = 0x1;   //delay in bits
    //Set data mask
    * MCASP_RXMASK = 0x0000ffff; /* Last 16 bit matter */
    //Active slots
    * MCASP_RXTDM_ptr                  = 0x0;
    MCASP_RXTDM_bits_ptr->TDMS0        = 0x1;
    MCASP_RXTDM_bits_ptr->TDMS1        = 0x1;
    //Setup serializers
    MCASP_SRCTL_RX_bits_ptr->SRMOD     = 0x2;   //receiver
    MCASP_SRCTL_RX_bits_ptr->DISMOD    = 0x0;   //tri-state
}
#endif
/*--- End of the function ----------------------------------------------------*/

#if (OPTIMIZED)
static inline void setup_mcasp_TX ()
{
start:
    * MCASP_GBLCTLX_ptr                 = 0x0;              //Reset TX part
    WAIT_REG_RESET(* MCASP_GBLCTLX_ptr);
    * MCASP_SRCTL_TX_ptr                = 0x0;              //Switch off TX serializer
    * MCASP_IRQCTLX                     = 0x0;              //All IRQs are disabled
    * MCASP_DMACTLX_ptr                 = 0x1;              //Disable DMA requests generation
    * MCASP_TXSTAT_ptr                  = 0xffffffff;       //Clear TX stat
    * MCASP_AHCLKXCTL_ptr               = AHCLKX_CTL_SETUP; //TX high speed clock
    * MCASP_ACLKXCTL_ptr                = ACLKX_CTL_SETUP;  //TX clock setup
    * MCASP_TXCLKCHK_ptr                = TX_CLK_CHK_SETUP; //TX clock control
    * MCASP_TXFMCTL_ptr                 = TX_FM_CTL_SETUP;  //TX frame format
    * MCASP_TXFMT_ptr                   = TX_FMT_SETUP;     //TX data format
    * MCASP_TXMASK                      = 0x0000ffff;       //Set TX data mask - last 16 bit matter
    * MCASP_TXTDM_ptr                   = 0b11;             //Active slots
    * MCASP_SRCTL_TX_ptr                = 0x1;              //Transmitter
}
/*--- End of the function ----------------------------------------------------*/

static inline void setup_mcasp_RX (void)
{
start:
    * MCASP_GBLCTLR_ptr                 = 0x0;              //Reset RX part
    WAIT_REG_RESET(* MCASP_GBLCTLR_ptr);
    * MCASP_SRCTL_RX_ptr                = 0x0;              //Switch off RX serializer
    * MCASP_IRQCTLR                     = 0x0;              //All IRQs are disabled
    * MCASP_DMACTLR_ptr                 = 0x1;              //Disable DMA requests generation
    * MCASP_RXSTAT_ptr                  = 0xffffffff;       //Clear RX stat
    * MCASP_AHCLKRCTL_ptr               = AHCLKR_CTL_SETUP; //RX high speed clock
//    * MCASP_ACLKRCTL_ptr                = ACLKR_CTL_SETUP;
    * MCASP_RXCLKCHK_ptr                = RX_CLK_CHK_SETUP; //RX clock control
    * MCASP_RXFMCTL_ptr                 = RX_FM_CTL_SETUP;
    * MCASP_RXFMT_ptr                   = RX_FMT_SETUP;
    * MCASP_RXMASK                      = 0x0000ffff;       //Set data mask - last 16 bit matter
    * MCASP_RXTDM_ptr                   = 0b11;             //Active slots
    * MCASP_SRCTL_RX_ptr                = 0x2;              //Receiver
}
#endif
/*--- End of the function ----------------------------------------------------*/

static int32_t start_mcasp_TX (void)
{
    uint32_t timeout_counter = 0;

    /** Start TX high-speed clock */
    MCASP_GBLCTLX_bits_ptr->XHCLKRST = 0x1;
    while (!MCASP_GBLCTLX_bits_ptr->XHCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_status_0_red();
#endif
            return -1;
        }
    };

    /** Start TX clock */
    MCASP_GBLCTLX_bits_ptr->XCLKRST = 0x1;
    timeout_counter = 0;
    while (!MCASP_GBLCTLX_bits_ptr->XCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_status_0_red();
#endif
            return -1;
        }
    };

    //Clear stat
    * MCASP_TXSTAT_ptr = 0xffffffff;

    /** Flush TX buffers to an empty state */
    MCASP_GBLCTLX_bits_ptr->XSRCLR = 0x1;
    timeout_counter = 0;
    while (!MCASP_GBLCTLX_bits_ptr->XSRCLR)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_status_0_red();
#endif
            return -1;
        }
    };

    MCASP_TXSTAT_bits_ptr->DATA = 0x1;

    /** Start TX state machine */
    MCASP_GBLCTLX_bits_ptr->XSMRST = 0x1;
    timeout_counter = 0;
    while (!MCASP_GBLCTLX_bits_ptr->XSMRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_status_0_red();
#endif
            return -1;
        }
    };

    //Write to the TX buffer to prevent underflow
    * MCASP_TXBUF_ptr = 0x0;
    * MCASP_TXBUF_ptr = 0x0;

    /** Start TX frame sync generator */
    MCASP_GBLCTLX_bits_ptr->XFRST = 0x1;
    timeout_counter = 0;
    while (!MCASP_GBLCTLX_bits_ptr->XFRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_status_0_red();
#endif
            return -1;
        }
    };
    return 0;
}
/*--- End of the function ----------------------------------------------------*/

static int32_t start_mcasp_RX (void)
{
    uint32_t timeout_counter = 0;

    /** Start RX high-speed clock */
    MCASP_GBLCTLR_bits_ptr->RHCLKRST = 0x1;
    while (!MCASP_GBLCTLR_bits_ptr->RHCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_status_1_red();
#endif
            return -1;
        }
    };

    /** Start RX clock */
    MCASP_GBLCTLR_bits_ptr->RCLKRST = 0x1;
    timeout_counter = 0;
    while (!MCASP_GBLCTLR_bits_ptr->RCLKRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_status_1_red();
#endif
            return -1;
        }
    };

    //Clear stat
    * MCASP_RXSTAT_ptr = 0xffffffff;

    /** Flush RX buffers to an empty state */
    MCASP_GBLCTLR_bits_ptr->RSRCLR = 0x1;
    timeout_counter = 0;
    while (!MCASP_GBLCTLR_bits_ptr->RSRCLR)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_status_1_red();
#endif
            return -1;
        }
    };

    MCASP_RXSTAT_bits_ptr->DATA = 0x1;

    /** Start RX state machine */
    MCASP_GBLCTLR_bits_ptr->RSMRST = 0x1;
    timeout_counter = 0;
    while (!MCASP_GBLCTLR_bits_ptr->RSMRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_status_1_red();
#endif
            return -1;
        }
    };

    //Read the RX buffer to prevent underflow
    uint32_t    dump = * MCASP_RXBUF_ptr;
                dump = * MCASP_RXBUF_ptr;

    /** Start frame sync generator */
    MCASP_GBLCTLR_bits_ptr->RFRST = 0x1;
    timeout_counter = 0;
    while (!MCASP_GBLCTLR_bits_ptr->RFRST)
    {
        service_buffs();
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
#if (DEBUG_LEDS_IDK_IDK_AM5728)
            set_status_1_red();
#endif
            return -1;
        }
    };
    return 0;
}
/*--- End of the function ----------------------------------------------------*/

void init_mcasp ()
{
    uint32_t pid = 0;
    pid = *(uint32_t *)MCASP_CFG_ADDR;
start:
    * MCASP_GBLCTL_ptr = 0;
    uint32_t timeout_counter = 0;
    while (* MCASP_GBLCTL_ptr)
    {
        timeout_counter++;
        if (timeout_counter>INIT_TIMEOUT_CNT)
        {
            goto start;
        }
    }
    //Switch off all the serializes
#if     (MCASP1 & IDK_AM5728)
    * MCASP_XRSRCTL_00_ptr   = 0x0;
    * MCASP_XRSRCTL_01_ptr   = 0x0;
    * MCASP_XRSRCTL_02_ptr   = 0x0;
    * MCASP_XRSRCTL_03_ptr   = 0x0;
    * MCASP_XRSRCTL_04_ptr   = 0x0;
    * MCASP_XRSRCTL_05_ptr   = 0x0;
    * MCASP_XRSRCTL_06_ptr   = 0x0;
    * MCASP_XRSRCTL_07_ptr   = 0x0;
    * MCASP_SRCTL_TX_ptr     = 0x0;
    * MCASP_XRSRCTL_09_ptr   = 0x0;
    * MCASP_XRSRCTL_10_ptr   = 0x0;
    * MCASP_XRSRCTL_11_ptr   = 0x0;
    * MCASP_XRSRCTL_12_ptr   = 0x0;
    * MCASP_XRSRCTL_13_ptr   = 0x0;
    * MCASP_SRCTL_RX_ptr     = 0x0;
    * MCASP_XRSRCTL_15_ptr   = 0x0;
#elif   (MCASP8 | BBB_AM335x)
    * MCASP_SRCTL_TX_ptr     = 0x0;
    * MCASP_SRCTL_RX_ptr     = 0x0;
    * MCASP_XRSRCTL_02_ptr   = 0x0;
    * MCASP_XRSRCTL_03_ptr   = 0x0;
#endif
    /* Global McASP setup */
    * MCASP_LBCTL_ptr        = 0x0; //No loop-back
    * MCASP_TXDITCTL_ptr     = 0x0; //No DIT

    #if MCASP_TX
        setup_mcasp_TX();
    #endif
    #if MCASP_RX
        setup_mcasp_RX();
    #endif
    setup_mcasp_pads();

    #if MCASP_TX
        if (start_mcasp_TX() < 0) { goto start; }
    #endif
    #if MCASP_RX
        if (start_mcasp_RX() < 0) { goto start; }
    #endif
}
/*--- End of the function ----------------------------------------------------*/
