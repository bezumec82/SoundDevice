#include "main.h"
#include "resource_table_0.h"

/* Start - p.4224 */
/* Industrial LED 0 */
#define CTRL_CORE_PAD_MCASP2_AXR4_ADDR      0x4A003714  //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
/* Industrial LED 1 */
#define CTRL_CORE_PAD_MCASP2_AXR5_ADDR      0x4A003718  //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
#define CTRL_CORE_PAD_MCASP2_AXR6_ADDR      0x4A00371C  //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
#define CTRL_CORE_PAD_MCASP2_AXR7_ADDR      0x4A003720  //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr
/* McASP1 */
#define CTRL_CORE_PAD_MCASP1_ACLKX_ADDR     0x4A0036A4  //IDK_572x -> AM57XX_PRU2ETH_MDCLK
#define CTRL_CORE_PAD_MCASP1_FSX_ADDR       0x4A0036A8  //IDK_572x -> AM57XX_PRU2ETH_MDDATA
#define CTRL_CORE_PAD_MCASP1_AXR8           0x4A0036D4  //IDK_572x -> AM57XX_PRU2ETH0_TXEN
#define CTRL_CORE_PAD_MCASP1_AXR14          0x4A0036EC  //IDK_572x -> AM57XX_PRU2ETH0_RXDV

/* Industrial LED 0 */
volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP2_AXR4_ptr  = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP2_AXR4_ADDR;   //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
/* Industrial LED 1 */
volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP2_AXR5_ptr  = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP2_AXR5_ADDR;   //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP2_AXR6_ptr  = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP2_AXR6_ADDR;   //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP2_AXR7_ptr  = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP2_AXR7_ADDR;   //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

/* McASP1 */
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP1_ACLKX_ADDR_ptr = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP1_ACLKX_ADDR;  //IDK_572x -> AM57XX_PRU2ETH_MDCLK
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP1_FSX_ADDR_ptr   = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP1_FSX_ADDR  ;  //IDK_572x -> AM57XX_PRU2ETH_MDDATA
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP1_AXR8_ptr       = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP1_AXR8      ;  //IDK_572x -> AM57XX_PRU2ETH0_TXEN
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP1_AXR14_ptr      = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP1_AXR14     ;  //IDK_572x -> AM57XX_PRU2ETH0_RXDV

#define MCASP2_AXR4_MUXMODE_gpio1_4         0xe     //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
#define MCASP2_AXR5_MUXMODE_gpio6_7         0xe     //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
#define MCASP2_AXR6_MUXMODE_gpio2_29        0xe     //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
#define MCASP2_AXR7_MUXMODE_gpio1_5         0xe     //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

#define MCASP1_ACLKX_MUXMODE_gpio7_31       0xe     //IDK_572x -> AM57XX_PRU2ETH_MDCLK
#define MCASP1_FSX_MUXMODE_gpio7_30         0xe     //IDK_572x -> AM57XX_PRU2ETH_MDDATA
#define MCASP1_AXR8_MUXMODE_gpio5_10        0xe     //IDK_572x -> AM57XX_PRU2ETH0_TXEN
#define MCASP1_AXR14_MUXMODE_gpio6_5        0xe     //IDK_572x -> AM57XX_PRU2ETH0_RXDV

#define MCASP1_ACLKX_MUXMODE_mcasp1_aclkx   0x0     //IDK_572x -> AM57XX_PRU2ETH_MDCLK
#define MCASP1_FSX_MUXMODE_mcasp1_fsx       0x0     //IDK_572x -> AM57XX_PRU2ETH_MDDATA
#define MCASP1_AXR8_MUXMODE_mcasp1_axr8     0x0     //IDK_572x -> AM57XX_PRU2ETH0_TXEN
#define MCASP1_AXR14_MUXMODE_mcasp1_axr14   0x0     //IDK_572x -> AM57XX_PRU2ETH0_RXDV

struct GPIO_SYSCONFIG * GPIO1_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO1_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO2_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO2_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO3_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO3_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO4_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO4_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO5_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO5_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO6_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO6_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO7_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO7_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO8_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO8_SYSCONFIG_ADDR;

struct GPIO_OE * GPIO1_OE_ptr = (struct GPIO_OE *)GPIO1_OE_ADDR;
struct GPIO_OE * GPIO2_OE_ptr = (struct GPIO_OE *)GPIO2_OE_ADDR;
struct GPIO_OE * GPIO3_OE_ptr = (struct GPIO_OE *)GPIO3_OE_ADDR;
struct GPIO_OE * GPIO4_OE_ptr = (struct GPIO_OE *)GPIO4_OE_ADDR;
struct GPIO_OE * GPIO5_OE_ptr = (struct GPIO_OE *)GPIO5_OE_ADDR;
struct GPIO_OE * GPIO6_OE_ptr = (struct GPIO_OE *)GPIO6_OE_ADDR;
struct GPIO_OE * GPIO7_OE_ptr = (struct GPIO_OE *)GPIO7_OE_ADDR;
struct GPIO_OE * GPIO8_OE_ptr = (struct GPIO_OE *)GPIO8_OE_ADDR;

struct GPIO_DATAOUT * GPIO1_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO1_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO2_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO2_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO3_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO3_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO4_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO4_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO5_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO5_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO6_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO6_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO7_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO7_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO8_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO8_DATAOUT_ADDR;

int main(void)
{
    /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

    /* 1 == no idle */
    GPIO1_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO2_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO3_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO4_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO6_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO7_SYSCONFIG_ptr->IDLEMODE = 0x1;

    /* 0 == out */
    GPIO1_OE_ptr->OUTPUTEN_4    = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
    GPIO6_OE_ptr->OUTPUTEN_7    = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    GPIO2_OE_ptr->OUTPUTEN_29   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    GPIO1_OE_ptr->OUTPUTEN_5    = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

    GPIO7_OE_ptr->OUTPUTEN_31   = 0x0;  //IDK_572x -> AM57XX_PRU2ETH_MDCLK              mcasp1_aclkx
    GPIO7_OE_ptr->OUTPUTEN_30   = 0x0;  //IDK_572x -> AM57XX_PRU2ETH_MDDATA             mcasp1_fsx
    GPIO5_OE_ptr->OUTPUTEN_10   = 0x0;  //IDK_572x -> AM57XX_PRU2ETH0_TXEN              mcasp1_axr8
    GPIO6_OE_ptr->OUTPUTEN_5    = 0x0;  //IDK_572x -> AM57XX_PRU2ETH0_RXDV              mcasp1_axr14

    /* Switch multiplexor */
    CTRL_CORE_PAD_MCASP2_AXR4_ptr       ->MUXMODE = MCASP2_AXR4_MUXMODE_gpio1_4 ;    //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
    CTRL_CORE_PAD_MCASP2_AXR5_ptr       ->MUXMODE = MCASP2_AXR5_MUXMODE_gpio6_7 ;    //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    CTRL_CORE_PAD_MCASP2_AXR6_ptr       ->MUXMODE = MCASP2_AXR6_MUXMODE_gpio2_29;    //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    CTRL_CORE_PAD_MCASP2_AXR7_ptr       ->MUXMODE = MCASP2_AXR7_MUXMODE_gpio1_5 ;    //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

    CTRL_CORE_PAD_MCASP1_ACLKX_ADDR_ptr ->MUXMODE = MCASP1_ACLKX_MUXMODE_gpio7_31;   //IDK_572x -> AM57XX_PRU2ETH_MDCLK              mcasp1_aclkx
    CTRL_CORE_PAD_MCASP1_FSX_ADDR_ptr   ->MUXMODE = MCASP1_FSX_MUXMODE_gpio7_30  ;   //IDK_572x -> AM57XX_PRU2ETH_MDDATA             mcasp1_fsx
    CTRL_CORE_PAD_MCASP1_AXR8_ptr       ->MUXMODE = MCASP1_AXR8_MUXMODE_gpio5_10 ;   //IDK_572x -> AM57XX_PRU2ETH0_TXEN              mcasp1_axr8
    CTRL_CORE_PAD_MCASP1_AXR14_ptr      ->MUXMODE = MCASP1_AXR14_MUXMODE_gpio6_5 ;   //IDK_572x -> AM57XX_PRU2ETH0_RXDV              mcasp1_axr14
    /* 0 == Enable */
    CTRL_CORE_PAD_MCASP2_AXR4_ptr       ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
    CTRL_CORE_PAD_MCASP2_AXR5_ptr       ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    CTRL_CORE_PAD_MCASP2_AXR6_ptr       ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    CTRL_CORE_PAD_MCASP2_AXR7_ptr       ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

    CTRL_CORE_PAD_MCASP1_ACLKX_ADDR_ptr ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_PRU2ETH_MDCLK              mcasp1_aclkx
    CTRL_CORE_PAD_MCASP1_FSX_ADDR_ptr   ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_PRU2ETH_MDDATA             mcasp1_fsx
    CTRL_CORE_PAD_MCASP1_AXR8_ptr       ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_PRU2ETH0_TXEN              mcasp1_axr8
    CTRL_CORE_PAD_MCASP1_AXR14_ptr      ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_PRU2ETH0_RXDV              mcasp1_axr14
    /* 0 == Pull down */
    CTRL_CORE_PAD_MCASP2_AXR4_ptr       ->PULLTYPESELECT = 0x1; //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
    CTRL_CORE_PAD_MCASP2_AXR5_ptr       ->PULLTYPESELECT = 0x1; //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    CTRL_CORE_PAD_MCASP2_AXR6_ptr       ->PULLTYPESELECT = 0x1; //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    CTRL_CORE_PAD_MCASP2_AXR7_ptr       ->PULLTYPESELECT = 0x1; //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

    CTRL_CORE_PAD_MCASP1_ACLKX_ADDR_ptr ->PULLTYPESELECT = 0x1; //IDK_572x -> AM57XX_PRU2ETH_MDCLK              mcasp1_aclkx
    CTRL_CORE_PAD_MCASP1_FSX_ADDR_ptr   ->PULLTYPESELECT = 0x1; //IDK_572x -> AM57XX_PRU2ETH_MDDATA             mcasp1_fsx
    CTRL_CORE_PAD_MCASP1_AXR8_ptr       ->PULLTYPESELECT = 0x1; //IDK_572x -> AM57XX_PRU2ETH0_TXEN              mcasp1_axr8
    CTRL_CORE_PAD_MCASP1_AXR14_ptr      ->PULLTYPESELECT = 0x1; //IDK_572x -> AM57XX_PRU2ETH0_RXDV              mcasp1_axr14
    /* 0 == Receive disabled */
    CTRL_CORE_PAD_MCASP2_AXR4_ptr       ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
    CTRL_CORE_PAD_MCASP2_AXR5_ptr       ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    CTRL_CORE_PAD_MCASP2_AXR6_ptr       ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    CTRL_CORE_PAD_MCASP2_AXR7_ptr       ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

    CTRL_CORE_PAD_MCASP1_ACLKX_ADDR_ptr ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_PRU2ETH_MDCLK              mcasp1_aclkx
    CTRL_CORE_PAD_MCASP1_FSX_ADDR_ptr   ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_PRU2ETH_MDDATA             mcasp1_fsx
    CTRL_CORE_PAD_MCASP1_AXR8_ptr       ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_PRU2ETH0_TXEN              mcasp1_axr8
    CTRL_CORE_PAD_MCASP1_AXR14_ptr      ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_PRU2ETH0_RXDV              mcasp1_axr14

    /* All off */
    GPIO1_DATAOUT_ptr->GPIO_4   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
    GPIO6_DATAOUT_ptr->GPIO_7   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    GPIO2_DATAOUT_ptr->GPIO_29  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    GPIO1_DATAOUT_ptr->GPIO_5   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

    GPIO7_DATAOUT_ptr->GPIO_31  = 0x0;  //IDK_572x -> AM57XX_PRU2ETH_MDCLK              mcasp1_aclkx
    GPIO7_DATAOUT_ptr->GPIO_30  = 0x0;  //IDK_572x -> AM57XX_PRU2ETH_MDDATA             mcasp1_fsx
    GPIO5_DATAOUT_ptr->GPIO_10  = 0x0;  //IDK_572x -> AM57XX_PRU2ETH0_TXEN              mcasp1_axr8
    GPIO6_DATAOUT_ptr->GPIO_5   = 0x0;  //IDK_572x -> AM57XX_PRU2ETH0_RXDV              mcasp1_axr14

    __delay_cycles(200000000);
    while (1) {
        /* McASP8 pins */
        GPIO1_DATAOUT_ptr->GPIO_4  = 0x1;   //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
        GPIO6_DATAOUT_ptr->GPIO_7  = 0x1;   //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
        GPIO2_DATAOUT_ptr->GPIO_29 = 0x1;   //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
        GPIO1_DATAOUT_ptr->GPIO_5  = 0x1;   //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr
        /* McASP1 pins */
        GPIO7_DATAOUT_ptr->GPIO_31  = 0x0;  //IDK_572x -> AM57XX_PRU2ETH_MDCLK              mcasp1_aclkx
        GPIO7_DATAOUT_ptr->GPIO_30  = 0x0;  //IDK_572x -> AM57XX_PRU2ETH_MDDATA             mcasp1_fsx
        GPIO5_DATAOUT_ptr->GPIO_10  = 0x0;  //IDK_572x -> AM57XX_PRU2ETH0_TXEN              mcasp1_axr8
        GPIO6_DATAOUT_ptr->GPIO_5   = 0x0;  //IDK_572x -> AM57XX_PRU2ETH0_RXDV              mcasp1_axr14
        __delay_cycles(2000000);
        /* McASP8 pins */
        GPIO1_DATAOUT_ptr->GPIO_4   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
        GPIO6_DATAOUT_ptr->GPIO_7   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
        GPIO2_DATAOUT_ptr->GPIO_29  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
        GPIO1_DATAOUT_ptr->GPIO_5   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr
        /* McASP1 pins */
        GPIO7_DATAOUT_ptr->GPIO_31  = 0x1;  //IDK_572x -> AM57XX_PRU2ETH_MDCLK              mcasp1_aclkx
        GPIO7_DATAOUT_ptr->GPIO_30  = 0x1;  //IDK_572x -> AM57XX_PRU2ETH_MDDATA             mcasp1_fsx
        GPIO5_DATAOUT_ptr->GPIO_10  = 0x1;  //IDK_572x -> AM57XX_PRU2ETH0_TXEN              mcasp1_axr8
        GPIO6_DATAOUT_ptr->GPIO_5   = 0x1;  //IDK_572x -> AM57XX_PRU2ETH0_RXDV              mcasp1_axr14
        __delay_cycles(2000000);

    } //end while (1)
}
