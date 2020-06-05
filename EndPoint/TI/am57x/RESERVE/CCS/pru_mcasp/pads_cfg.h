#ifndef __PADS_CFG_H__
#define __PADS_CFG_H__

#include "main.h"

/* Status LED 0 */
#define CTRL_CORE_PAD_VIN1A_D7_ADDR         0x4A003510  //IDK_572x -> AM57XX_STATUSLED0_GRN
#define CTRL_CORE_PAD_VIN1A_D8_ADDR         0x4A003514  //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
#define CTRL_CORE_PAD_VIN2A_VSYNC0_ADDR     0x4A003564  //IDK_572x -> AM57XX_STATUSLED0_RED

/* Status LED 1 */
#define CTRL_CORE_PAD_VIN1A_D6_ADDR         0x4A00350C  //IDK_572x -> AM57XX_STATUSLED1_RED
#define CTRL_CORE_PAD_UART1_RXD_ADDR        0x4A0037E0  //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
#define CTRL_CORE_PAD_UART1_TXD_ADDR        0x4A0037E4  //IDK_572x -> AM57XX_STATUSLED1_GRN

/* Industrial LED 0 */
#define CTRL_CORE_PAD_MCASP2_AXR4_ADDR      0x4A003714  //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
#define CTRL_CORE_PAD_VIN1A_D5_ADDR         0x4A003508  //IDK_572x -> AM57XX_INDETHER_LED0_GRN
#define CTRL_CORE_PAD_XREF_CLK2_ADDR        0x4A00369C  //IDK_572x -> AM57XX_INDETHER_LED0_RED

/* Industrial LED 1 */
#define CTRL_CORE_PAD_MCASP2_AXR5_ADDR      0x4A003718  //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
#define CTRL_CORE_PAD_MCASP2_AXR6_ADDR      0x4A00371C  //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
#define CTRL_CORE_PAD_MCASP2_AXR7_ADDR      0x4A003720  //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

/* Industrial LED 2 */
#define CTRL_CORE_PAD_SPI1_D1_ADDR          0x4A0037A8  //IDK_572x -> AM57XX_INDETHER_LED2_GRN
#define CTRL_CORE_PAD_SPI1_D0_ADDR          0x4A0037AC  //IDK_572x -> AM57XX_INDETHER_LED2_RED
#define CTRL_CORE_PAD_SPI1_CS0_ADDR         0x4A0037B0  //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)

/* Industrial LED 3 */
#define CTRL_CORE_PAD_VIN1A_D13_ADDR        0x4A003528  //IDK_572x -> AM57XX_INDETHER_LED3_GRN
#define CTRL_CORE_PAD_VIN1A_D14_ADDR        0x4A00352C  //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
#define CTRL_CORE_PAD_SPI1_CS1_ADDR         0x4A0037B4  //IDK_572x -> AM57XX_INDETHER_LED3_RED

/* McASP1 */
#define CTRL_CORE_PAD_MCASP1_ACLKX_ADDR     0x4A0036A4  //IDK572x -> AM57XX_PRU2ETH_MDCLK
#define CTRL_CORE_PAD_MCASP1_FSX_ADDR       0x4A0036A8  //IDK572x -> AM57XX_PRU2ETH_MDDATA
#define CTRL_CORE_PAD_MCASP1_AXR8           0x4A0036D4  //IDK572x -> AM57XX_PRU2ETH0_TXEN
#define CTRL_CORE_PAD_MCASP1_AXR14          0x4A0036EC  //IDK572x -> AM57XX_PRU2ETH0_RXDV

void config_mcasp8_pads(void);
void config_mcasp1_pads(void);

void setup_gpio_pads(void);

void set_status_0_red (void);
void set_status_1_red (void);

void set_industial_2_red (void);
void set_industial_3_red (void);

void set_status_0_green (void);
void set_status_1_green (void);

void set_industial_2_green (void);
void set_industial_3_green (void);


void set_status_0_blue (void);
void set_status_1_blue (void);

void set_industial_2_blue (void);
void set_industial_3_blue (void);

#endif
