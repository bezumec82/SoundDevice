#ifndef __McASP_REGS_H__
#define __McASP_REGS_H__

#include "mcasp_mod.h"

/*! @name Revision Identification Register
 * @{ */
#define McASP_REV                       0x0
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Power Idle SYSCONFIG Register
 * @{ */
#define McASP_PWRIDLESYSCONFIG          0x4
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Pin Function Register
 * @{ */
#define McASP_PFUNC                     0x10
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Pin Direction Register
 * @{ */
#define McASP_PDIR                      0x14
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Pin Data Output Register
 * @{ */
#define McASP_PDOUT                     0x18
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Pin Data Input Register
 * @{ */
#define McASP_PDIN                      0x1C
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Pin Data Clear Register
 * @{ */
#define McASP_PDCLR                     0x20
/*! @} ------------------------------------------------------------------------------------------ */


/*! @name Global Control Register
 * @{ */
#define McASP_GBLCTL                    0x44
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Receiver Global Control Register
 * @{ */
#define McASP_RGBLCTL                   0x60
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Transmitter Global Control Register
 * @{ */
#define McASP_XGBLCTL                   0xA0
/*! @} ------------------------------------------------------------------------------------------ */



#if AMUTE_PRESENT
        /*! @name Audio Mute Control Register
         * @{ */
        #define McASP_AMUTE                     0x48
        /*! @} ------------------------------------------------------------------------------------------ */
#endif
/*! @name Digital Loopback Control Register
 * @{ */
#define McASP_DLBCTL                    0x4C
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name DIT Mode Control Register
 * @{ */
#define McASP_DITCTL                    0x50
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name Receive Format Unit Bit Mask Register
 * @{ */
#define McASP_RMASK                     0x64
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Receive Bit Stream Format Register
 * @{ */
#define McASP_RFMT                      0x68
/*! @} ------------------------------------------------------------------------------------------ */


/*! @name Receive Frame Sync Control Register
 * @{ */
#define McASP_AFSRCTL                   0x6C
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Receive Clock Control Register
 * @{ */
#define McASP_ACLKRCTL                  0x70
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Receive Clock Control Register
 * @{ */
#define McASP_AHCLKRCTL                 0x74
/*! @} ------------------------------------------------------------------------------------------ */


/*! @name Receive TDM Time Slot 0-31 Register
 * @{ */
#define McASP_RTDM                      0x78
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Receiver Interrupt Control Register
 * @{ */
#define McASP_RINTCTL                   0x7C
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Receiver Status Register
 * @{ */
#define McASP_RSTAT                     0x80
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Current Receive TDM Time Slot Register
 * @{ */
#define McASP_RSLOT                     0x84
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Receive Clock Check Control Register
 * @{ */
#define McASP_RCLKCHK                   0x88
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Receiver DMA Event Control Register
 * @{ */
#define McASP_REVTCTL                   0x8C
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name Transmitter Global Control Register
 * @{ */
#define McASP_XMASK                     0xA4
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Transmit Bit Stream Format Register
 * @{ */
#define McASP_XFMT                      0xA8
/*! @} ------------------------------------------------------------------------------------------ */


/*! @name Transmit Frame Sync Control Register
 * @{ */
#define McASP_AFSXCTL                   0xAC
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Transmit Clock Control Register
 * @{ */
#define McASP_ACLKXCTL                  0xB0
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Transmit High-Frequency Clock Control Register
 * @{ */
#define McASP_AHCLKXCTL                 0xB4
/*! @} ------------------------------------------------------------------------------------------ */


/*! @name Transmit TDM Time Slot 0-31 Register
 * @{ */
#define McASP_XTDM                      0xB8
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Transmitter Interrupt Control Register
 * @{ */
#define McASP_XINTCTL                   0xBC
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Transmitter Status Register
 * @{ */
#define McASP_XSTAT                     0xC0
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Current Transmit TDM Time Slot Register
 * @{ */
#define McASP_XSLOT                     0xC4
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Transmit Clock Check Control Register
 * @{ */
#define McASP_XCLKCHK                   0xC8
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Transmitter DMA Event Control Register
 * @{ */
#define McASP_XEVTCTL                   0xCC
/*! @} ------------------------------------------------------------------------------------------ */
#if (DIT_MODE)
        /*! @name Left (Even TDM Time Slot) Channel Status Registers (DIT Mode)
         * @{ */
        #define McASP_DITCSRA_0                 0x100
        #define McASP_DITCSRA_1                 0x104
        #define McASP_DITCSRA_2                 0x108
        #define McASP_DITCSRA_3                 0x10C
        #define McASP_DITCSRA_4                 0x110
        #define McASP_DITCSRA_5                 0x114
        /*! @} ------------------------------------------------------------------------------------------ */
        /*! @name Right (Odd TDM Time Slot) Channel Status Registers (DIT Mode)
         * @{ */
        #define McASP_DITCSRB_0                 0x118
        #define McASP_DITCSRB_1                 0x11C
        #define McASP_DITCSRB_2                 0x120
        #define McASP_DITCSRB_3                 0x124
        #define McASP_DITCSRB_4                 0x128
        #define McASP_DITCSRB_5                 0x12C
        /*! @} ------------------------------------------------------------------------------------------ */
        /*! @name Left (Even TDM Time Slot) Channel User Data Registers (DIT Mode)
         * @{ */
        #define McASP_DITUDRA_0                 0x130
        #define McASP_DITUDRA_1                 0x134
        #define McASP_DITUDRA_2                 0x138
        #define McASP_DITUDRA_3                 0x13C
        #define McASP_DITUDRA_4                 0x140
        #define McASP_DITUDRA_5                 0x144
        /*! @} ------------------------------------------------------------------------------------------ */
        /*! @name Right (Odd TDM Time Slot) Channel User Data Registers (DIT Mode)
         * @{ */
        #define McASP_DITUDRB_0                 0x148
        #define McASP_DITUDRB_1                 0x14C
        #define McASP_DITUDRB_2                 0x150
        #define McASP_DITUDRB_3                 0x154
        #define McASP_DITUDRB_4                 0x158
        #define McASP_DITUDRB_5                 0x15C
        /*! @} ------------------------------------------------------------------------------------------ */
#endif
/*! @name Serializer Control Registers
 * @{ */
#define McASP_SRCTL_0                   0x180
#define McASP_SRCTL_1                   0x184
#define McASP_SRCTL_2                   0x188
#define McASP_SRCTL_3                   0x18C
#define McASP_SRCTL_4                   0x190
#define McASP_SRCTL_5                   0x194

/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Transmit Buffer Register for Serializers
 * @{ */
#define McASP_XBUF_0                    0x200
#define McASP_XBUF_1                    0x204
#define McASP_XBUF_2                    0x208
#define McASP_XBUF_3                    0x20C
#define McASP_XBUF_4                    0x210
#define McASP_XBUF_5                    0x214

/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Receive Buffer Register for Serializers
 * @{ */
#define McASP_RBUF_0                    0x280
#define McASP_RBUF_1                    0x284
#define McASP_RBUF_2                    0x288
#define McASP_RBUF_3                    0x28C
#define McASP_RBUF_4                    0x290
#define McASP_RBUF_5                    0x294
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Write FIFO Control Register
 * @{ */
#define McASP_WFIFOCTL                  0x1000
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Write FIFO Status Register
 * @{ */
#define McASP_WFIFOSTS                  0x1004
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Read FIFO Control Register
 * @{ */
#define McASP_RFIFOCTL                  0x1008
/*! @} ------------------------------------------------------------------------------------------ */
/*! @name Read FIFO Status Register
 * @{ */
#define McASP_RFIFOSTS                  0x100C
/*! @} ------------------------------------------------------------------------------------------ */

/* Register access macros */
#define mcasp_readl(base, reg_off)            __raw_readl (base + reg_off)
#define mcasp_writel(base, reg_off, value)    __raw_writel((value), base + reg_off)

#endif /* __McASP_REGS_H__ */
