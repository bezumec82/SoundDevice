#ifndef __MCASP_REGS__
#define __MCASP_REGS__

#include "main.h"
#if     (IDK_AM5728)
    #define MCASP1_CFG_ADDR     0x48460000
    #define MCASP8_CFG_ADDR     0x4847C000
#elif   (BBB_AM335x)
    #define MCASP0_CFG_ADDR     0x48038000
    #define MCASP1_CFG_ADDR     0x4803C000
#endif

#define MCASP_PFUNC_OFF     0x10
#define MCASP_PDIR_OFF      0x14

#define MCASP_GBLCTL_OFF    0x44
#define MCASP_GBLCTLX_OFF   0xA0
#define MCASP_GBLCTLR_OFF   0x60

#define MCASP_LBCTL_OFF     0x4C
#define MCASP_TXDITCTL_OFF  0x50
#define MCASP_TXMASK_OFF    0xA4

#define MCASP_RXMASK_OFF    0x64

#define MCASP_TXFMT_OFF     0xA8
#define MCASP_RXFMT_OFF     0x68

#define MCASP_TXFMCTL_OFF   0xAC
#define MCASP_RXFMCTL_OFF   0x6C

#define MCASP_ACLKXCTL_OFF  0xB0
#define MCASP_ACLKRCTL_OFF  0x70

#define MCASP_AHCLKXCTL_OFF 0xB4
#define MCASP_AHCLKRCTL_OFF 0x74

#define MCASP_TXTDM_OFF     0xB8
#define MCASP_RXTDM_OFF     0x78

#define MCASP_IRQCTLX_OFF   0xBC //MCASP_EVTCTLX
#define MCASP_IRQCTLR_OFF   0x7C //MCASP_EVTCTLR

#define MCASP_TXSTAT_OFF    0xC0
#define MCASP_RXSTAT_OFF    0x80

#define MCASP_TXTDMSLOT_OFF 0xC4
#define MCASP_RXTDMSLOT_OFF 0x84

#define MCASP_TXCLKCHK_OFF  0xC8
#define MCASP_RXCLKCHK_OFF  0x88

#define MCASP_DMACTLX_OFF   0xCC //MCASP_XEVTCTL
#define MCASP_DMACTLR_OFF   0x8C //MCASP_REVTCTL

#define MCASP_XRSRCTL_00_OFF  (uint32_t)(0x180 + (0x4 * 0 ))
#define MCASP_XRSRCTL_01_OFF  (uint32_t)(0x180 + (0x4 * 1 ))
#define MCASP_XRSRCTL_02_OFF  (uint32_t)(0x180 + (0x4 * 2 ))
#define MCASP_XRSRCTL_03_OFF  (uint32_t)(0x180 + (0x4 * 3 ))
#define MCASP_XRSRCTL_04_OFF  (uint32_t)(0x180 + (0x4 * 4 ))
#define MCASP_XRSRCTL_05_OFF  (uint32_t)(0x180 + (0x4 * 5 ))
#define MCASP_XRSRCTL_06_OFF  (uint32_t)(0x180 + (0x4 * 6 ))
#define MCASP_XRSRCTL_07_OFF  (uint32_t)(0x180 + (0x4 * 7 ))
#define MCASP_XRSRCTL_08_OFF  (uint32_t)(0x180 + (0x4 * 8 ))
#define MCASP_XRSRCTL_09_OFF  (uint32_t)(0x180 + (0x4 * 9 ))
#define MCASP_XRSRCTL_10_OFF  (uint32_t)(0x180 + (0x4 * 10))
#define MCASP_XRSRCTL_11_OFF  (uint32_t)(0x180 + (0x4 * 11))
#define MCASP_XRSRCTL_12_OFF  (uint32_t)(0x180 + (0x4 * 12))
#define MCASP_XRSRCTL_13_OFF  (uint32_t)(0x180 + (0x4 * 13))
#define MCASP_XRSRCTL_14_OFF  (uint32_t)(0x180 + (0x4 * 14))
#define MCASP_XRSRCTL_15_OFF  (uint32_t)(0x180 + (0x4 * 15))

#define MCASP_TXBUF_00_OFF    (uint32_t)(0x200 + (0x4 * 0 ))
#define MCASP_TXBUF_01_OFF    (uint32_t)(0x200 + (0x4 * 1 ))
#define MCASP_TXBUF_02_OFF    (uint32_t)(0x200 + (0x4 * 2 ))
#define MCASP_TXBUF_03_OFF    (uint32_t)(0x200 + (0x4 * 3 ))
#define MCASP_TXBUF_04_OFF    (uint32_t)(0x200 + (0x4 * 4 ))
#define MCASP_TXBUF_05_OFF    (uint32_t)(0x200 + (0x4 * 5 ))
#define MCASP_TXBUF_06_OFF    (uint32_t)(0x200 + (0x4 * 6 ))
#define MCASP_TXBUF_07_OFF    (uint32_t)(0x200 + (0x4 * 7 ))
#define MCASP_TXBUF_08_OFF    (uint32_t)(0x200 + (0x4 * 8 ))
#define MCASP_TXBUF_09_OFF    (uint32_t)(0x200 + (0x4 * 9 ))
#define MCASP_TXBUF_10_OFF    (uint32_t)(0x200 + (0x4 * 10))
#define MCASP_TXBUF_11_OFF    (uint32_t)(0x200 + (0x4 * 11))
#define MCASP_TXBUF_12_OFF    (uint32_t)(0x200 + (0x4 * 12))
#define MCASP_TXBUF_13_OFF    (uint32_t)(0x200 + (0x4 * 13))
#define MCASP_TXBUF_14_OFF    (uint32_t)(0x200 + (0x4 * 14))
#define MCASP_TXBUF_15_OFF    (uint32_t)(0x200 + (0x4 * 15))

#define MCASP_RXBUF_00_OFF    (uint32_t)(0x280 + (0x4 * 0 ))
#define MCASP_RXBUF_01_OFF    (uint32_t)(0x280 + (0x4 * 1 ))
#define MCASP_RXBUF_02_OFF    (uint32_t)(0x280 + (0x4 * 2 ))
#define MCASP_RXBUF_03_OFF    (uint32_t)(0x280 + (0x4 * 3 ))
#define MCASP_RXBUF_04_OFF    (uint32_t)(0x280 + (0x4 * 4 ))
#define MCASP_RXBUF_05_OFF    (uint32_t)(0x280 + (0x4 * 5 ))
#define MCASP_RXBUF_06_OFF    (uint32_t)(0x280 + (0x4 * 6 ))
#define MCASP_RXBUF_07_OFF    (uint32_t)(0x280 + (0x4 * 7 ))
#define MCASP_RXBUF_08_OFF    (uint32_t)(0x280 + (0x4 * 8 ))
#define MCASP_RXBUF_09_OFF    (uint32_t)(0x280 + (0x4 * 9 ))
#define MCASP_RXBUF_10_OFF    (uint32_t)(0x280 + (0x4 * 10))
#define MCASP_RXBUF_11_OFF    (uint32_t)(0x280 + (0x4 * 11))
#define MCASP_RXBUF_12_OFF    (uint32_t)(0x280 + (0x4 * 12))
#define MCASP_RXBUF_13_OFF    (uint32_t)(0x280 + (0x4 * 13))
#define MCASP_RXBUF_14_OFF    (uint32_t)(0x280 + (0x4 * 14))
#define MCASP_RXBUF_15_OFF    (uint32_t)(0x280 + (0x4 * 15))

struct MCASP_PFUNC {
    uint32_t AXR0           : 1;
    uint32_t AXR1           : 1;
    uint32_t AXR2           : 1;
    uint32_t AXR3           : 1;
    uint32_t AXR4           : 1;
    uint32_t AXR5           : 1;
    uint32_t AXR6           : 1;
    uint32_t AXR7           : 1;
    uint32_t AXR8           : 1;
    uint32_t AXR9           : 1;
    uint32_t AXR10          : 1;
    uint32_t AXR11          : 1;
    uint32_t AXR12          : 1;
    uint32_t AXR13          : 1;
    uint32_t AXR14          : 1;
    uint32_t AXR15          : 1;
    uint32_t RESERVED_16    : 10;

    uint32_t ACLKX          : 1;
    uint32_t AHCLKX         : 1;
    uint32_t AFSX           : 1;
    uint32_t ACLKR          : 1;
    uint32_t AHCLKR         : 1;
    uint32_t AFSR           : 1;
};

struct MCASP_PDIR {
    uint32_t AXR0           : 1;
    uint32_t AXR1           : 1;
    uint32_t AXR2           : 1;
    uint32_t AXR3           : 1;
    uint32_t AXR4           : 1;
    uint32_t AXR5           : 1;
    uint32_t AXR6           : 1;
    uint32_t AXR7           : 1;
    uint32_t AXR8           : 1;
    uint32_t AXR9           : 1;
    uint32_t AXR10          : 1;
    uint32_t AXR11          : 1;
    uint32_t AXR12          : 1;
    uint32_t AXR13          : 1;
    uint32_t AXR14          : 1;
    uint32_t AXR15          : 1;
    uint32_t RESERVED_16    : 10;

    uint32_t ACLKX          : 1;
    uint32_t AHCLKX         : 1;
    uint32_t AFSX           : 1;
    uint32_t ACLKR          : 1;
    uint32_t RESERVED_30    : 1;
    uint32_t AFSR           : 1;
};

struct MCASP_GBLCTL {
    uint32_t RCLKRST        :1;
    uint32_t RHCLKRST       :1;
    uint32_t RSRCLR         :1;
    uint32_t RSMRST         :1;
    uint32_t RFRST          :1;

    uint32_t RESERVED_5     :3;

    uint32_t XCLKRST        :1;
    uint32_t XHCLKRST       :1;
    uint32_t XSRCLR         :1; //Transmit serializer clear enable bit.
    uint32_t XSMRST         :1;
    uint32_t XFRST          :1;

    uint32_t RESERVED_13    :19;
};

struct MCASP_TXMASK {
    uint32_t XMASK0         : 1;
    uint32_t XMASK1         : 1;
    uint32_t XMASK2         : 1;
    uint32_t XMASK3         : 1;
    uint32_t XMASK4         : 1;
    uint32_t XMASK5         : 1;
    uint32_t XMASK6         : 1;
    uint32_t XMASK7         : 1;
    uint32_t XMASK8         : 1;
    uint32_t XMASK9         : 1;

    uint32_t XMASK10        : 1;
    uint32_t XMASK11        : 1;
    uint32_t XMASK12        : 1;
    uint32_t XMASK13        : 1;
    uint32_t XMASK14        : 1;
    uint32_t XMASK15        : 1;
    uint32_t XMASK16        : 1;
    uint32_t XMASK17        : 1;
    uint32_t XMASK18        : 1;
    uint32_t XMASK19        : 1;

    uint32_t XMASK20        : 1;
    uint32_t XMASK21        : 1;
    uint32_t XMASK22        : 1;
    uint32_t XMASK23        : 1;
    uint32_t XMASK24        : 1;
    uint32_t XMASK25        : 1;
    uint32_t XMASK26        : 1;
    uint32_t XMASK27        : 1;
    uint32_t XMASK28        : 1;
    uint32_t XMASK29        : 1;

    uint32_t XMASK30        : 1;
    uint32_t XMASK31        : 1;
};

struct MCASP_XFMT {
    uint32_t ROT            : 3;
    uint32_t BUSEL          : 1;
    uint32_t SSZ            : 4;
    uint32_t PBIT           : 5;
    uint32_t PAD            : 2;
    uint32_t RVRS           : 1;
    uint32_t DATDLY         : 2;
    uint32_t RESERVED_18    : 14;
};

struct MCASP_FMCTL {
    uint32_t FSP            : 1;
    uint32_t FSM            : 1;
    uint32_t RESERVED_2     : 2;
    uint32_t FWID           : 1;
    uint32_t RESERVED_5     : 2;
    uint32_t MOD            : 9;
    uint32_t RESERVED_16    : 16;
};

struct MCASP_ACLKCTL {
    uint32_t CLKDIV         : 5;
    uint32_t CLKM           : 1;
    uint32_t ASYNC          : 1; //Reserved for MCASP_ACLKRCTL
    uint32_t CLKP           : 1;
    uint32_t RESERVED_8     : 8;
    uint32_t CLKADJ         : 2;
    uint32_t ADJBUSY        : 1;
    uint32_t DIVBUSY        : 1;
    uint32_t BUSY           : 1;
    uint32_t RESERVED_21    : 11;
};

struct MCASP_AHCLKCTL {
    uint32_t HCLKDIV       : 12;
    uint32_t RESERVED_12    : 2;
    uint32_t HCLKP         : 1;
    uint32_t HCLKM         : 1;
    uint32_t HCLKADJ       : 2;
    uint32_t ADJBUSY        : 1;
    uint32_t DIVBUSY        : 1;
    uint32_t BUSY           : 1;
    uint32_t RESERVED_21    : 11;
};

struct MCASP_TDMS {
    unsigned TDMS0 : 1;
    unsigned TDMS1 : 1;
    unsigned TDMS2 : 1;
    unsigned TDMS3 : 1;
    unsigned TDMS4 : 1;
    unsigned TDMS5 : 1;
    unsigned TDMS6 : 1;
    unsigned TDMS7 : 1;
    unsigned TDMS8 : 1;
    unsigned TDMS9 : 1;

    unsigned TDMS10 : 1;
    unsigned TDMS11 : 1;
    unsigned TDMS12 : 1;
    unsigned TDMS13 : 1;
    unsigned TDMS14 : 1;
    unsigned TDMS15 : 1;
    unsigned TDMS16 : 1;
    unsigned TDMS17 : 1;
    unsigned TDMS18 : 1;
    unsigned TDMS19 : 1;

    unsigned TDMS20 : 1;
    unsigned TDMS21 : 1;
    unsigned TDMS22 : 1;
    unsigned TDMS23 : 1;
    unsigned TDMS24 : 1;
    unsigned TDMS25 : 1;
    unsigned TDMS26 : 1;
    unsigned TDMS27 : 1;
    unsigned TDMS28 : 1;
    unsigned TDMS29 : 1;

    unsigned TDMS30 : 1;
    unsigned TDMS31 : 1;
};

struct MCASP_IRQCTL {
    uint32_t RN             : 1;
    uint32_t SYNCERR        : 1;
    uint32_t CKFAIL         : 1;
    uint32_t DMAERR         : 1;
    uint32_t LAST           : 1;
    uint32_t DATA           : 1;
    uint32_t RESERVED_6     : 1;
    uint32_t STAFRM         : 1;
    uint32_t RESERVED_8     : 24;
};

struct MCASP_STAT {
    uint32_t RN             : 1;
    uint32_t SYNCERR        : 1;
    uint32_t CKFAIL         : 1;
    uint32_t TDMSLOT        : 1;
    uint32_t LAST           : 1;
    uint32_t DATA           : 1;
    uint32_t STAFRM         : 1;
    uint32_t DMAERR         : 1;
    uint32_t ERR            : 1;
    uint32_t RESERVED_9     : 23;
};

struct MCASP_TDMSLOT {
    uint32_t SLOTCNT       : 9;
    uint32_t RESERVED_9     : 22;
};

struct MCASP_CLKCHK {
    uint32_t PS             : 4;
    uint32_t RESERVED_4     : 4;
    uint32_t MIN            : 8;
    uint32_t MAX            : 8;
    uint32_t CNT            : 8;
};

struct MCASP_DMACTL {
    uint32_t DATDMA         : 1;
    uint32_t RESERVED_1     : 31;
};

/* ... */
struct MCASP_SRCTL {
    uint32_t SRMOD          : 2;
    uint32_t DISMOD         : 2;
    uint32_t XRDY           : 1;
    uint32_t RRDY           : 1;
    uint32_t RESERVED_6     : 26;
};




#endif
