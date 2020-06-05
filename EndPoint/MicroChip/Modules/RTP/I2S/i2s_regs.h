#ifndef __I2S_REGS_H__
#define __I2S_REGS_H__

#include "i2s_mod.h"

/*! @name I2SC Control Register
 * @{ */
#define I2SC_CR_OFF                 0x000
                                    
#define I2SC_CR_RXEN_OFF            0
#define I2SC_CR_RXDIS_OFF           1
#define I2SC_CR_CKEN_OFF            2
#define I2SC_CR_CKDIS_OFF           3
#define I2SC_CR_TXEN_OFF            4
#define I2SC_CR_TXDIS_OFF           5
#define I2SC_CR_SWRST_OFF           7
                                    
#define I2SC_CR_RXEN_SIZE           1
#define I2SC_CR_RXDIS_SIZE          1
#define I2SC_CR_CKEN_SIZE           1
#define I2SC_CR_CKDIS_SIZE          1
#define I2SC_CR_TXEN_SIZE           1
#define I2SC_CR_TXDIS_SIZE          1
#define I2SC_CR_OFFSWRST_SIZE       1
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name I2SC Mode Register
 * @{ */
#define I2SC_MR_OFF                 0x004

#define I2SC_MR_MODE_OFF            0
#define I2SC_MR_DATALENGTH_OFF      2
#define I2SC_MR_FORMAT_OFF          6
#define I2SC_MR_RXMONO_OFF          8
#define I2SC_MR_RXLOOP_OFF          10
#define I2SC_MR_TXMONO_OFF          12
#define I2SC_MR_TXSAME_OFF          14
#define I2SC_MR_IMCKDIV_OFF         16
#define I2SC_MR_IMCKFS_OFF          24
#define I2SC_MR_IMCKMODE_OFF        30
#define I2SC_MR_IWS_OFF             31
                                    
#define I2SC_MR_MODE_SIZE           1
#define I2SC_MR_DATALENGTH_SIZE     3
#define I2SC_MR_FORMAT_SIZE         2
#define I2SC_MR_RXMONO_SIZE         1
#define I2SC_MR_RXLOOP_SIZE         1
#define I2SC_MR_TXMONO_SIZE         1
#define I2SC_MR_TXSAME_SIZE         1
#define I2SC_MR_IMCKDIV_SIZE        6
#define I2SC_MR_IMCKFS_SIZE         6
#define I2SC_MR_IMCKMODE_SIZE       1
#define I2SC_MR_OFFIWS_SIZE         1
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name I2SC Status Register
 * @{ */
#define I2SC_SR_OFF                 0x008

#define I2SC_SR_RXEN_OFF            0
#define I2SC_SR_RXRDY_OFF           1
#define I2SC_SR_RXOR_OFF            2
#define I2SC_SR_TXEN_OFF            4
#define I2SC_SR_TXRDY_OFF           5
#define I2SC_SR_TXUR_OFF            6
#define I2SC_SR_RXORCH_OFF          8
#define I2SC_SR_TXURCH_OFF          20

#define I2SC_SR_RXEN_SIZE           0
#define I2SC_SR_RXRDY_SIZE          1
#define I2SC_SR_RXOR_SIZE           1
#define I2SC_SR_TXEN_SIZE           1
#define I2SC_SR_TXRDY_SIZE          1
#define I2SC_SR_TXUR_SIZE           1
#define I2SC_SR_RXORCH_SIZE         2
#define I2SC_SR_TXURCH_SIZE         2
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name I2SC Status Clear Register
 * @{ */
#define I2SC_SCR_OFF                0x00C

#define I2SC_SCR_RXOR_OFF           2
#define I2SC_SCR_TXUR_OFF           6
#define I2SC_SCR_RXORCH_OFF         8
#define I2SC_SCR_TXURCH_OFF         20

#define I2SC_SCR_RXOR_SIZE          1
#define I2SC_SCR_TXUR_SIZE          1
#define I2SC_SCR_RXORCH_SIZE        2
#define I2SC_SCR_TXURCH_SIZE        2
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name I2SC Status Set Register
 * @{ */
#define I2SC_SSR_OFF                0x010

#define I2SC_SSR_RXOR_OFF           2
#define I2SC_SSR_TXUR_OFF           6
#define I2SC_SSR_RXORCH_OFF         8
#define I2SC_SSR_TXURCH_OFF         20

#define I2SC_SSR_RXOR_SIZE          1
#define I2SC_SSR_TXUR_SIZE          1
#define I2SC_SSR_RXORCH_SIZE        2
#define I2SC_SSR_TXURCH_SIZE        2
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name I2SC Interrupt Enable Register
 * @{ */
#define I2SC_IER_OFF                0x014

#define I2SC_IER_RXRDY_OFF          1
#define I2SC_IER_RXOR_OFF           2
#define I2SC_IER_TXRDY_OFF          5
#define I2SC_IER_TXUR_OFF           6

#define I2SC_IER_RXRDY_SIZE         1
#define I2SC_IER_RXOR_SIZE          1
#define I2SC_IER_TXRDY_SIZE         1
#define I2SC_IER_TXUR_SIZE          1
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name I2SC Interrupt Disable Register
 * @{ */
#define I2SC_IDR_OFF                0x018

#define I2SC_IDR_RXRDY_OFF          1
#define I2SC_IDR_RXOR_OFF           2
#define I2SC_IDR_TXRDY_OFF          5
#define I2SC_IDR_TXUR_OFF           6
              
#define I2SC_IDR_RXRDY_SIZE         1
#define I2SC_IDR_RXOR_SIZE          1
#define I2SC_IDR_TXRDY_SIZE         1
#define I2SC_IDR_TXUR_SIZE          1
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name I2SC Interrupt Mask Register
 * @{ */
#define I2SC_IMR_OFF                0x01C

#define I2SC_IMR_RXRDY_OFF          1
#define I2SC_IMR_RXOR_OFF           2
#define I2SC_IMR_TXRDY_OFF          5
#define I2SC_IMR_TXUR_OFF           6
              
#define I2SC_IMR_RXRDY_SIZE         1
#define I2SC_IMR_RXOR_SIZE          1
#define I2SC_IMR_TXRDY_SIZE         1
#define I2SC_IMR_TXUR_SIZE          1
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name I2SC Receiver Holding Register
 * @{ */
#define I2SC_RHR_OFF                0x020
#define I2SC_RHR_SIZE               32
/*! @} ------------------------------------------------------------------------------------------ */

/*! @name I2SC Transmitter Holding Register
 * @{ */
#define I2SC_THR_OFF                0x024
#define I2SC_THR_SIZE               32
/*! @} ------------------------------------------------------------------------------------------ */

#define XDMAC_CSA_D1_CH0    0xF0004060
#define XDMAC_CSA_D1_CH1    0xF00040A0
/*...*/
#define XDMAC_CSA_D1_CH15   0xF0004420

#define XDMAC_CSA_STEP  (XDMAC_CSA_D1_CH1 - XDMAC_CSA_D1_CH0)

#define XDMAC_CSA_D0_CH0    0xF0010060
#define XDMAC_CSA_D0_CH15   0xF0010420


/* Register access macros */
#define i2s_readl(base, reg_off)            __raw_readl (base + reg_off)
#define i2s_writel(base, reg_off, value)    __raw_writel((value), base + reg_off)

#endif /* __I2S_REGS_H__ */
