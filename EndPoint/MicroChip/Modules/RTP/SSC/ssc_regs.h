#ifndef __SSC_REGS_H__
#define __SSC_REGS_H__

#include "ssc_mod.h"

/*! @name SSC Control Register 
 * @{ */
#define SSC_CR_OFFSET               0x00000000
#define SSC_CR_RXEN_OFFSET          0   /*!< Receive Enable */
#define SSC_CR_RXDIS_OFFSET         1   /*!< Receive Disable */
#define SSC_CR_TXEN_OFFSET          8   /*!< Transmit Enable */
#define SSC_CR_TXDIS_OFFSET         9   /*!< Transmit Disable */
#define SSC_CR_SWRST_OFFSET         15  /*!< Software Reset */

#define SSC_CR_RXEN_SIZE            1
#define SSC_CR_RXDIS_SIZE           1
#define SSC_CR_TXEN_SIZE            1
#define SSC_CR_TXDIS_SIZE           1
#define SSC_CR_SWRST_SIZE           1
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Clock Mode Register
 * @{ */
#define SSC_CMR_OFFSET              0x00000004
#define SSC_CMR_DIV_OFFSET          0   /*!< Clock Divider */
#define SSC_CMR_DIV_SIZE            12
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Receive Clock Mode Register
 * @{ */
#define SSC_RCMR_OFFSET             0x00000010
#define SSC_RCMR_CKS_OFFSET         0   /*!< Receive Clock Selection */
#define SSC_RCMR_CKO_OFFSET         2   /*!< Receive Clock Output Mode Selection */
#define SSC_RCMR_CKI_OFFSET         5   /*!< Receive Clock Inversion */
#define SSC_RCMR_CKG_OFFSET         6   /*!< Receive Clock Gating Selection */
#define SSC_RCMR_START_OFFSET       8   /*!< Receive Start Selection */
#define SSC_RCMR_STOP_OFFSET        12  /*!< Receive Stop Selection */
#define SSC_RCMR_STTDLY_OFFSET      16  /*!< Receive Start Delay */
#define SSC_RCMR_PERIOD_OFFSET      24  /*!< Receive Period Divider Selection */

#define SSC_RCMR_CKS_SIZE           2
#define SSC_RCMR_CKO_SIZE           3
#define SSC_RCMR_CKI_SIZE           1
#define SSC_RCMR_CKG_SIZE           2
#define SSC_RCMR_START_SIZE         4
#define SSC_RCMR_STOP_SIZE          1
#define SSC_RCMR_STTDLY_SIZE        8
#define SSC_RCMR_PERIOD_SIZE        8
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Receive Frame Mode Register
 * @{ */
#define SSC_RFMR_OFFSET             0x00000014
#define SSC_RFMR_DATLEN_OFFSET      0   /*!< Data Length */
#define SSC_RFMR_LOOP_OFFSET        5   /*!< Loop Mode */
#define SSC_RFMR_MSBF_OFFSET        7   /*!< Most Significant Bit First */
#define SSC_RFMR_DATNB_OFFSET       8   /*!< Data Number per Frame */
#define SSC_RFMR_FSLEN_OFFSET       16  /*!< Receive Frame Sync Length */
#define SSC_RFMR_FSOS_OFFSET        20  /*!< Transmit Frame Sync Output Selection */
#define SSC_RFMR_FSEDGE_OFFSET      24  /*!< Frame Sync Edge Detection */
#define SSC_RFMR_FSLEN_EXT_OFFSET   28

#define SSC_RFMR_DATLEN_SIZE        5
#define SSC_RFMR_LOOP_SIZE          1
#define SSC_RFMR_MSBF_SIZE          1
#define SSC_RFMR_DATNB_SIZE         4
#define SSC_RFMR_FSLEN_SIZE         4
#define SSC_RFMR_FSOS_SIZE          3
#define SSC_RFMR_FSEDGE_SIZE        1
#define SSC_RFMR_FSLEN_EXT_SIZE     4
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Transmit Clock Mode Register
 * @{ */
#define SSC_TCMR_OFFSET             0x00000018

#define SSC_TCMR_CKS_OFFSET         0   /*!< Transmit Clock Selection */
#define SSC_TCMR_CKO_OFFSET         2   /*!< Transmit Clock Output Mode Selection */
#define SSC_TCMR_CKI_OFFSET         5   /*!< Transmit Clock Inversion */
#define SSC_TCMR_CKG_OFFSET         6   /*!< Transmit Clock Gating Selection */
#define SSC_TCMR_START_OFFSET       8   /*!< Transmit Start Selection */
#define SSC_TCMR_STTDLY_OFFSET      16  /*!< Transmit Start Delay */
#define SSC_TCMR_PERIOD_OFFSET      24  /*!< Transmit Period Divider Selection */

#define SSC_TCMR_CKS_SIZE           2
#define SSC_TCMR_CKO_SIZE           3
#define SSC_TCMR_CKI_SIZE           1
#define SSC_TCMR_CKG_SIZE           2
#define SSC_TCMR_START_SIZE         4
#define SSC_TCMR_STTDLY_SIZE        8
#define SSC_TCMR_PERIOD_SIZE        8
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Transmit Frame Mode Register
 * @{ */
#define SSC_TFMR_OFFSET             0x0000001c

#define SSC_TFMR_DATLEN_OFFSET      0   /*!< Data Length */
#define SSC_TFMR_DATDEF_OFFSET      5   /*!< Data Default Value */
#define SSC_TFMR_MSBF_OFFSET        7   /*!< Most Significant Bit First */
#define SSC_TFMR_DATNB_OFFSET       8   /*!< Data Number per Frame */
#define SSC_TFMR_FSLEN_OFFSET       16  /*!< Transmit Frame Sync Length */
#define SSC_TFMR_FSOS_OFFSET        20  /*!< Transmit Frame Sync Output Selection */
#define SSC_TFMR_FSDEN_OFFSET       23  /*!< Frame Sync Data Enable */
#define SSC_TFMR_FSEDGE_OFFSET      24  /*!< Frame Sync Edge Detection */
#define SSC_TFMR_FSLEN_EXT_OFFSET   28  /*!< FSLEN Field Extension */

#define SSC_TFMR_DATLEN_SIZE        5
#define SSC_TFMR_DATDEF_SIZE        1
#define SSC_TFMR_MSBF_SIZE          1
#define SSC_TFMR_DATNB_SIZE         4
#define SSC_TFMR_FSLEN_SIZE         4
#define SSC_TFMR_FSOS_SIZE          3
#define SSC_TFMR_FSDEN_SIZE         1
#define SSC_TFMR_FSEDGE_SIZE        1
#define SSC_TFMR_FSLEN_EXT_SIZE     4
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Receive Hold Register
 * @{ */
#define SSC_RHR_OFFSET              0x00000020
#define SSC_RHR_RDAT_SIZE           32
#define SSC_RHR_RDAT_OFFSET         0
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Transmit Hold Register
 * @{ */
#define SSC_THR_OFFSET              0x00000024
#define SSC_THR_TDAT_SIZE           32
#define SSC_THR_TDAT_OFFSET         0
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Receive Synchronization Holding Register
 * @{ */
#define SSC_RSHR_OFFSET             0x00000030
#define SSC_RSHR_RSDAT_SIZE         16
#define SSC_RSHR_RSDAT_OFFSET       0
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Transmit Synchronization Holding Register
 * @{ */
#define SSC_TSHR_OFFSET             0x00000034
#define SSC_TSHR_TSDAT_SIZE         16
#define SSC_TSHR_RSDAT_OFFSET       0
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Receive Compare 0 Register
 * @{ */
#define SSC_RC0R_OFFSET             0x00000038
#define SSC_RC0R_CP0_SIZE           16
#define SSC_RC0R_CP0_OFFSET         0
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Receive Compare 1 Register
 * @{ */
#define SSC_RC1R_OFFSET             0x0000003c
#define SSC_RC1R_CP1_SIZE           16
#define SSC_RC1R_CP1_OFFSET         0
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Status Register
 * @{ */
#define SSC_SR_OFFSET               0x00000040

#define SSC_SR_TXRDY_OFFSET         0   /*!< Transmit Ready */
#define SSC_SR_TXEMPTY_OFFSET       1   /*!< Transmit Empty */
#define SSC_SR_RXRDY_OFFSET         4   /*!< Receive Ready */
#define SSC_SR_OVRUN_OFFSET         5   /*!< Receive Overrun */
#define SSC_SR_CP0_OFFSET           8   /*!< Compare 0 */
#define SSC_SR_CP1_OFFSET           9   /*!< Compare 1 */
#define SSC_SR_TXSYN_OFFSET         10  /*!< Transmit Sync */
#define SSC_SR_RXSYN_OFFSET         11  /*!< Receive Sync */
#define SSC_SR_TXEN_OFFSET          16  /*!< Transmit Enable */
#define SSC_SR_RXEN_OFFSET          17  /*!< Receive Enable */

#define SSC_SR_TXRDY_SIZE           1
#define SSC_SR_TXEMPTY_SIZE         1
#define SSC_SR_RXRDY_SIZE           1
#define SSC_SR_OVRUN_SIZE           1
#define SSC_SR_CP0_SIZE             1
#define SSC_SR_CP1_SIZE             1
#define SSC_SR_TXSYN_SIZE           1
#define SSC_SR_RXSYN_SIZE           1
#define SSC_SR_TXEN_SIZE            1
#define SSC_SR_RXEN_SIZE            1

//#define SSC_SR_ENDRX_SIZE           1
//#define SSC_SR_ENDRX_OFFSET         6
//#define SSC_SR_ENDTX_SIZE           1
//#define SSC_SR_ENDTX_OFFSET         2
//#define SSC_SR_RXBUFF_SIZE          1
//#define SSC_SR_RXBUFF_OFFSET        7
//#define SSC_SR_TXBUFE_SIZE          1
//#define SSC_SR_TXBUFE_OFFSET        3


/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Interrupt Enable Register
 * @{ */
#define SSC_IER_OFFSET              0x00000044

#define SSC_IER_TXRDY_OFFSET        0   /*!< Transmit Ready Interrupt Enable */
#define SSC_IER_TXEMPTY_OFFSET      1   /*!< Transmit Empty Interrupt Enable */
#define SSC_IER_RXRDY_OFFSET        4   /*!< Receive Ready Interrupt Enable */
#define SSC_IER_OVRUN_OFFSET        5   /*!< Receive Overrun Interrupt Enable */
#define SSC_IER_CP0_OFFSET          8   /*!< Compare 0 Interrupt Enable */
#define SSC_IER_CP1_OFFSET          9   /*!< Compare 1 Interrupt Enable */
#define SSC_IER_TXSYN_OFFSET        10  /*!< Tx Sync Interrupt Enable */
#define SSC_IER_RXSYN_OFFSET        11  /*!< Rx Sync Interrupt Enable */

#define SSC_IER_TXRDY_SIZE          1
#define SSC_IER_TXEMPTY_SIZE        1
#define SSC_IER_RXRDY_SIZE          1
#define SSC_IER_OVRUN_SIZE          1
#define SSC_IER_CP0_SIZE            1
#define SSC_IER_CP1_SIZE            1
#define SSC_IER_TXSYN_SIZE          1
#define SSC_IER_RXSYN_SIZE          1

//#define SSC_IER_ENDRX_SIZE          1
//#define SSC_IER_ENDRX_OFFSET        6
//#define SSC_IER_ENDTX_SIZE          1
//#define SSC_IER_ENDTX_OFFSET        2
//#define SSC_IER_RXBUFF_SIZE         1
//#define SSC_IER_RXBUFF_OFFSET       7
//#define SSC_IER_TXBUFE_SIZE         1
//#define SSC_IER_TXBUFE_OFFSET       3
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Interrupt Disable Register
 * @{ */
#define SSC_IDR_OFFSET              0x00000048

#define SSC_IDR_TXRDY_OFFSET        0   /*!< Transmit Ready Interrupt Disable */
#define SSC_IDR_TXEMPTY_OFFSET      1   /*!< Transmit Empty Interrupt Disable */
#define SSC_IDR_RXRDY_OFFSET        4   /*!< Receive Ready Interrupt Disable */
#define SSC_IDR_OVRUN_OFFSET        5   /*!< Receive Overrun Interrupt Disable */
#define SSC_IDR_CP0_OFFSET          8   /*!< Compare 0 Interrupt Disable */
#define SSC_IDR_CP1_OFFSET          9   /*!< Compare 1 Interrupt Disable */
#define SSC_IDR_TXSYN_OFFSET        10  /*!< Tx Sync Interrupt Disable */
#define SSC_IDR_RXSYN_OFFSET        11  /*!< Rx Sync Interrupt Disable */

#define SSC_IDR_TXRDY_SIZE          1
#define SSC_IDR_TXEMPTY_SIZE        1
#define SSC_IDR_RXRDY_SIZE          1
#define SSC_IDR_OVRUN_SIZE          1
#define SSC_IDR_CP0_SIZE            1
#define SSC_IDR_CP1_SIZE            1
#define SSC_IDR_TXSYN_SIZE          1
#define SSC_IDR_RXSYN_SIZE          1

//#define SSC_IDR_ENDRX_SIZE          1
//#define SSC_IDR_ENDRX_OFFSET        6
//#define SSC_IDR_ENDTX_SIZE          1
//#define SSC_IDR_ENDTX_OFFSET        2
//#define SSC_IDR_RXBUFF_SIZE         1
//#define SSC_IDR_RXBUFF_OFFSET       7
//#define SSC_IDR_TXBUFE_SIZE         1
//#define SSC_IDR_TXBUFE_OFFSET       3
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Interrupt Mask Register 
 * @{ */
#define SSC_IMR_OFFSET              0x0000004c

#define SSC_IMR_TXRDY_OFFSET        0   /*!< Transmit Ready Interrupt Mask */
#define SSC_IMR_TXEMPTY_OFFSET      1   /*!< Transmit Empty Interrupt Mask */
#define SSC_IMR_RXRDY_OFFSET        4   /*!< Receive Ready Interrupt Mask */
#define SSC_IMR_OVRUN_OFFSET        5   /*!< Receive Overrun Interrupt Mask */
#define SSC_IMR_CP0_OFFSET          8   /*!< Compare 0 Interrupt Mask */
#define SSC_IMR_CP1_OFFSET          9   /*!< Compare 1 Interrupt Mask */
#define SSC_IMR_TXSYN_OFFSET        10  /*!< Tx Sync Interrupt Mask */

#define SSC_IMR_TXRDY_SIZE          1
#define SSC_IMR_TXEMPTY_SIZE        1
#define SSC_IMR_RXRDY_SIZE          1
#define SSC_IMR_RXSYN_SIZE          1
#define SSC_IMR_OVRUN_SIZE          1
#define SSC_IMR_CP0_SIZE            1
#define SSC_IMR_CP1_SIZE            1
#define SSC_IMR_TXSYN_SIZE          1

//#define SSC_IMR_ENDRX_SIZE          1
//#define SSC_IMR_ENDRX_OFFSET        6
//#define SSC_IMR_ENDTX_SIZE          1
//#define SSC_IMR_ENDTX_OFFSET        2
//#define SSC_IMR_RXBUFF_SIZE         1
//#define SSC_IMR_RXBUFF_OFFSET       7
//#define SSC_IMR_RXSYN_OFFSET        11
//#define SSC_IMR_TXBUFE_SIZE         1
//#define SSC_IMR_TXBUFE_OFFSET       3
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Write Protection Mode Register
 * @{ */
#define SSC_WPMR_OFFSET             0x000000E4

#define SSC_WPMR_WPEN_OFFSET        0   /*!< Write Protection Enable */
#define SSC_WPMR_WPKEY_OFFSET       8   /*!< Write Protection Key (0x535343) */

#define SSC_WPMR_WPEN_SIZE          1
#define SSC_WPMR_WPKEY_SIZE         24
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name SSC Write Protection Status Register
 * @{ */

#define SSC_WPSR_OFFSET             0x000000E8

#define SSC_WPSR_WPVS_OFFSET        0   /*!< Write Protection Violation Status */
#define SSC_WPSR_WPVSRC_OFFSET      8   /*!< Write Protect Violation Source */
#define SSC_WPSR_WPVS_SIZE          1
#define SSC_WPSR_WPVSRC_SIZE        16
/*! @} ------------------------------------------------------------------------------------------ */

/* Bit manipulation macros */
#define SSC_BIT(name)                           \
    (1 << SSC_##name##_OFFSET)
#define SSC_BF(name, value)                     \
    (((value) & ((1 << SSC_##name##_SIZE) - 1)) \
     << SSC_##name##_OFFSET)
#define SSC_BFEXT(name, value)                  \
    (((value) >> SSC_##name##_OFFSET)           \
     & ((1 << SSC_##name##_SIZE) - 1))
#define SSC_BFINS(name, value, old)             \
    (((old) & ~(((1 << SSC_##name##_SIZE) - 1)  \
    << SSC_##name##_OFFSET)) | SSC_BF(name, value))

/* Register access macros */
#define ssc_readl(base, reg_off)            __raw_readl (base + reg_off)
#define ssc_writel(base, reg_off, value)    __raw_writel((value), base + reg_off)

#endif /* __SSC_REGS_H__ */
