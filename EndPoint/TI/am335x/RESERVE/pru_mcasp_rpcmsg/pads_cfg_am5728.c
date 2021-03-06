#include "pads_cfg.h"

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

#if (DEBUG_LEDS_IDK_IDK_AM5728)
    /* Status LED 0 */
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_VIN1A_D7_ptr     = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_VIN1A_D7_ADDR;      //IDK_572x -> AM57XX_STATUSLED0_GRN
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_VIN1A_D8_ptr     = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_VIN1A_D8_ADDR;      //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_VIN2A_VSYNC0_ptr = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_VIN2A_VSYNC0_ADDR;  //IDK_572x -> AM57XX_STATUSLED0_RED
    /* Status LED 1 */
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_VIN1A_D6_ptr     = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_VIN1A_D6_ADDR;      //IDK_572x -> AM57XX_STATUSLED1_RED
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_UART1_RXD_ptr    = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_UART1_RXD_ADDR;     //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_UART1_TXD_ptr    = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_UART1_TXD_ADDR;     //IDK_572x -> AM57XX_STATUSLED1_GRN
    /* Industrial LED 0 */
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_VIN1A_D5_ptr     = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_VIN1A_D5_ADDR;      //IDK_572x -> AM57XX_INDETHER_LED0_GRN
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_XREF_CLK2_ptr    = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_XREF_CLK2_ADDR;     //IDK_572x -> AM57XX_INDETHER_LED0_RED
    /* Industrial LED 2 */
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_SPI1_D1_ptr      = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_SPI1_D1_ADDR;       //IDK_572x -> AM57XX_INDETHER_LED2_GRN
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_SPI1_D0_ptr      = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_SPI1_D0_ADDR;       //IDK_572x -> AM57XX_INDETHER_LED2_RED
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_SPI1_CS0_ptr     = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_SPI1_CS0_ADDR;      //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)
    /* Industrial LED 3 */
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_VIN1A_D13_ptr    = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_VIN1A_D13_ADDR;     //IDK_572x -> AM57XX_INDETHER_LED3_GRN
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_VIN1A_D14_ptr    = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_VIN1A_D14_ADDR;     //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
    static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_SPI1_CS1_ptr     = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_SPI1_CS1_ADDR;      //IDK_572x -> AM57XX_INDETHER_LED3_RED
#endif /* DEBUG_LEDS_IDK_IDK_AM5728 */
/* Industrial LED 0 */
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP2_AXR4_ptr  = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP2_AXR4_ADDR;   //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
/* Industrial LED 1 */
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP2_AXR5_ptr  = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP2_AXR5_ADDR;   //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP2_AXR6_ptr  = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP2_AXR6_ADDR;   //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP2_AXR7_ptr  = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP2_AXR7_ADDR;   //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr
/* McASP1 */
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP1_ACLKX_ADDR_ptr = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP1_ACLKX_ADDR;  //IDK572x -> AM57XX_PRU2ETH_MDCLK
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP1_FSX_ADDR_ptr   = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP1_FSX_ADDR  ;  //IDK572x -> AM57XX_PRU2ETH_MDDATA
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP1_AXR8_ptr       = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP1_AXR8      ;  //IDK572x -> AM57XX_PRU2ETH0_TXEN
static volatile  struct CTRL_CORE_PAD * CTRL_CORE_PAD_MCASP1_AXR14_ptr      = (struct CTRL_CORE_PAD *)CTRL_CORE_PAD_MCASP1_AXR14     ;  //IDK572x -> AM57XX_PRU2ETH0_RXDV

#if (DEBUG_LEDS_IDK_IDK_AM5728)
#define VIN1A_D7_MUXMODE_gpio3_11       0xe     //IDK_572x -> AM57XX_STATUSLED0_GRN
#define VIN1A_D8_MUXMODE_gpio3_12       0xe     //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
#define VIN2A_VSYNC0_MUXMODE_gpio4_0    0xe     //IDK_572x -> AM57XX_STATUSLED0_RED

#define VIN1A_D6_MUXMODE_gpio3_10       0xe     //IDK_572x -> AM57XX_STATUSLED1_RED
#define UART1_RXD_MUXMODE_gpio7_22      0xe     //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
#define UART1_TXD_MUXMODE_gpio7_23      0xe     //IDK_572x -> AM57XX_STATUSLED1_GRN

#define MCASP2_AXR4_MUXMODE_gpio1_4     0xe     //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
#define VIN1A_D5_MUXMODE_gpio3_9        0xe     //IDK_572x -> AM57XX_INDETHER_LED0_GRN
#define XREF_CLK2_MUXMODE_gpio6_19      0xe     //IDK_572x -> AM57XX_INDETHER_LED0_RED

#define MCASP2_AXR5_MUXMODE_gpio6_7     0xe     //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
#define MCASP2_AXR6_MUXMODE_gpio2_29    0xe     //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
#define MCASP2_AXR7_MUXMODE_gpio1_5     0xe     //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

#define SPI1_D1_MUXMODE_gpio7_8         0xe     //IDK_572x -> AM57XX_INDETHER_LED2_GRN
#define SPI1_D0_MUXMODE_gpio7_9         0xe     //IDK_572x -> AM57XX_INDETHER_LED2_RED
#define SPI1_CS0_MUXMODE_gpio7_10       0xe     //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)

#define VIN1A_D13_MUXMODE_gpio3_17      0xe     //IDK_572x -> AM57XX_INDETHER_LED3_GRN
#define VIN1A_D14_MUXMODE_gpio3_18      0xe     //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
#define SPI1_CS1_MUXMODE_gpio7_11       0xe     //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
#endif /* DEBUG_LEDS_IDK_IDK_AM5728 */

#define MCASP2_AXR4_MUXMODE_mcasp8_axr0     0x1     //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
#define MCASP2_AXR5_MUXMODE_mcasp8_axr1     0x1     //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
#define MCASP2_AXR6_MUXMODE_mcasp8_aclkx    0x1     //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
#define MCASP2_AXR7_MUXMODE_mcasp8_fsx      0x1     //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

#define MCASP1_ACLKX_MUXMODE_mcasp1_aclkx   0x0
#define MCASP1_FSX_MUXMODE_mcasp1_fsx       0x0
#define MCASP1_AXR8_MUXMODE_mcasp1_axr8     0x0
#define MCASP1_AXR14_MUXMODE_mcasp1_axr14   0x0

void config_mcasp8_pads_idk_am5728(void)
{
    /* Switch multiplexor */
    CTRL_CORE_PAD_MCASP2_AXR4_ptr  ->MUXMODE = MCASP2_AXR4_MUXMODE_mcasp8_axr0 ;    //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
    CTRL_CORE_PAD_MCASP2_AXR5_ptr  ->MUXMODE = MCASP2_AXR5_MUXMODE_mcasp8_axr1 ;    //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    CTRL_CORE_PAD_MCASP2_AXR6_ptr  ->MUXMODE = MCASP2_AXR6_MUXMODE_mcasp8_aclkx;    //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    CTRL_CORE_PAD_MCASP2_AXR7_ptr  ->MUXMODE = MCASP2_AXR7_MUXMODE_mcasp8_fsx ;     //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr
    /* 0 == Enable */
    CTRL_CORE_PAD_MCASP2_AXR4_ptr  ->PULLUDENABLE = 0x0;    //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
    CTRL_CORE_PAD_MCASP2_AXR5_ptr  ->PULLUDENABLE = 0x0;    //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    CTRL_CORE_PAD_MCASP2_AXR6_ptr  ->PULLUDENABLE = 0x0;    //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    CTRL_CORE_PAD_MCASP2_AXR7_ptr  ->PULLUDENABLE = 0x0;    //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr
    /* 0 == Pull down */
    CTRL_CORE_PAD_MCASP2_AXR4_ptr  ->PULLTYPESELECT = 0x1;  //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
    CTRL_CORE_PAD_MCASP2_AXR5_ptr  ->PULLTYPESELECT = 0x1;  //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    CTRL_CORE_PAD_MCASP2_AXR6_ptr  ->PULLTYPESELECT = 0x1;  //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    CTRL_CORE_PAD_MCASP2_AXR7_ptr  ->PULLTYPESELECT = 0x1;  //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr
    /* For the mcaspx_aclkx, mcaspx_ahclkx and mcaspx_aclkr signals to work properly,
     * the INPUTENABLE bit of the appropriate CTRL_CORE_PAD_x registers should be set to 0x1 because of retiming purposes. */
    CTRL_CORE_PAD_MCASP2_AXR6_ptr   ->INPUTENABLE = 0x1;    //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    CTRL_CORE_PAD_MCASP2_AXR5_ptr   ->INPUTENABLE = 0x1;    //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
#if (CDC_IS_MASTER)
    CTRL_CORE_PAD_MCASP2_AXR7_ptr   ->INPUTENABLE = 0x1;    //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr
#endif
}

void config_mcasp1_pads_idk_am5728(void)
{
    /* Switch multiplexor */
    CTRL_CORE_PAD_MCASP1_ACLKX_ADDR_ptr ->MUXMODE           = MCASP1_ACLKX_MUXMODE_mcasp1_aclkx;    //IDK572x -> AM57XX_PRU2ETH_MDCLK
    CTRL_CORE_PAD_MCASP1_FSX_ADDR_ptr   ->MUXMODE           = MCASP1_FSX_MUXMODE_mcasp1_fsx;        //IDK572x -> AM57XX_PRU2ETH_MDDATA
    CTRL_CORE_PAD_MCASP1_AXR8_ptr       ->MUXMODE           = MCASP1_AXR8_MUXMODE_mcasp1_axr8;      //IDK572x -> AM57XX_PRU2ETH0_TXEN
    CTRL_CORE_PAD_MCASP1_AXR14_ptr      ->MUXMODE           = MCASP1_AXR14_MUXMODE_mcasp1_axr14;    //IDK572x -> AM57XX_PRU2ETH0_RXDV
    /* 0 == Enable */
    CTRL_CORE_PAD_MCASP1_ACLKX_ADDR_ptr ->PULLUDENABLE      = 0x0; //IDK572x -> AM57XX_PRU2ETH_MDCLK
    CTRL_CORE_PAD_MCASP1_FSX_ADDR_ptr   ->PULLUDENABLE      = 0x0; //IDK572x -> AM57XX_PRU2ETH_MDDATA
    CTRL_CORE_PAD_MCASP1_AXR8_ptr       ->PULLUDENABLE      = 0x0; //IDK572x -> AM57XX_PRU2ETH0_TXEN
    CTRL_CORE_PAD_MCASP1_AXR14_ptr      ->PULLUDENABLE      = 0x0; //IDK572x -> AM57XX_PRU2ETH0_RXDV
    /* 0 == Pull down */
    CTRL_CORE_PAD_MCASP1_ACLKX_ADDR_ptr ->PULLTYPESELECT    = 0x1; //IDK572x -> AM57XX_PRU2ETH_MDCLK
    CTRL_CORE_PAD_MCASP1_FSX_ADDR_ptr   ->PULLTYPESELECT    = 0x1; //IDK572x -> AM57XX_PRU2ETH_MDDATA
    CTRL_CORE_PAD_MCASP1_AXR8_ptr       ->PULLTYPESELECT    = 0x1; //IDK572x -> AM57XX_PRU2ETH0_TXEN
    CTRL_CORE_PAD_MCASP1_AXR14_ptr      ->PULLTYPESELECT    = 0x1; //IDK572x -> AM57XX_PRU2ETH0_RXDV
    /* For the mcaspx_aclkx, mcaspx_ahclkx and mcaspx_aclkr signals to work properly,
     * the INPUTENABLE bit of the appropriate CTRL_CORE_PAD_x registers should be set to 0x1 because of retiming purposes. */
    CTRL_CORE_PAD_MCASP1_ACLKX_ADDR_ptr     ->INPUTENABLE = 0x1;
    #if (CRISS_CROSS)
        CTRL_CORE_PAD_MCASP1_AXR8_ptr       ->INPUTENABLE = 0x1;
    #else
        CTRL_CORE_PAD_MCASP1_AXR14_ptr      ->INPUTENABLE = 0x1;
    #endif
    #if (CDC_IS_MASTER)
        CTRL_CORE_PAD_MCASP1_FSX_ADDR_ptr   ->INPUTENABLE = 0x1;
    #endif
}

#if (DEBUG_LEDS_IDK_IDK_AM5728)
struct GPIO_SYSCONFIG * GPIO1_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO1_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO2_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO2_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO3_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO3_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO4_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO4_SYSCONFIG_ADDR;
//struct GPIO_SYSCONFIG * GPIO5_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO5_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO6_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO6_SYSCONFIG_ADDR;
struct GPIO_SYSCONFIG * GPIO7_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO7_SYSCONFIG_ADDR;
//struct GPIO_SYSCONFIG * GPIO8_SYSCONFIG_ptr = (struct GPIO_SYSCONFIG *)GPIO8_SYSCONFIG_ADDR;

struct GPIO_OE * GPIO1_OE_ptr = (struct GPIO_OE *)GPIO1_OE_ADDR;
struct GPIO_OE * GPIO2_OE_ptr = (struct GPIO_OE *)GPIO2_OE_ADDR;
struct GPIO_OE * GPIO3_OE_ptr = (struct GPIO_OE *)GPIO3_OE_ADDR;
struct GPIO_OE * GPIO4_OE_ptr = (struct GPIO_OE *)GPIO4_OE_ADDR;
//struct GPIO_OE * GPIO5_OE_ptr = (struct GPIO_OE *)GPIO5_OE_ADDR;
struct GPIO_OE * GPIO6_OE_ptr = (struct GPIO_OE *)GPIO6_OE_ADDR;
struct GPIO_OE * GPIO7_OE_ptr = (struct GPIO_OE *)GPIO7_OE_ADDR;
//struct GPIO_OE * GPIO8_OE_ptr = (struct GPIO_OE *)GPIO8_OE_ADDR;


struct GPIO_DATAOUT * GPIO1_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO1_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO2_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO2_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO3_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO3_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO4_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO4_DATAOUT_ADDR;
//struct GPIO_DATAOUT * GPIO5_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO5_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO6_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO6_DATAOUT_ADDR;
struct GPIO_DATAOUT * GPIO7_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO7_DATAOUT_ADDR;
//struct GPIO_DATAOUT * GPIO8_DATAOUT_ptr = (struct GPIO_DATAOUT *)GPIO8_DATAOUT_ADDR;

void setup_gpio_pads(void)
{
    /* 1 == no idle */
    GPIO1_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO2_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO3_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO4_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO6_SYSCONFIG_ptr->IDLEMODE = 0x1;

    /* 0 == out */
    GPIO3_OE_ptr->OUTPUTEN_11 = 0x0;    //IDK_572x -> AM57XX_STATUSLED0_GRN
    GPIO3_OE_ptr->OUTPUTEN_12 = 0x0;    //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
    GPIO4_OE_ptr->OUTPUTEN_0 = 0x0;     //IDK_572x -> AM57XX_STATUSLED0_RED

    GPIO3_OE_ptr->OUTPUTEN_10 = 0x0;    //IDK_572x -> AM57XX_STATUSLED1_RED
    GPIO7_OE_ptr->OUTPUTEN_22 = 0x0;    //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
    GPIO7_OE_ptr->OUTPUTEN_23 = 0x0;    //IDK_572x -> AM57XX_STATUSLED1_GRN

    GPIO1_OE_ptr->OUTPUTEN_4 = 0x0;     //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)
    GPIO3_OE_ptr->OUTPUTEN_9 = 0x0;     //IDK_572x -> AM57XX_INDETHER_LED0_GRN
    GPIO6_OE_ptr->OUTPUTEN_19 = 0x0;    //IDK_572x -> AM57XX_INDETHER_LED0_RED

    GPIO6_OE_ptr->OUTPUTEN_7 = 0x0;     //IDK_572x -> AM57XX_INDETHER_LED1_RED
    GPIO2_OE_ptr->OUTPUTEN_29 = 0x0;    //IDK_572x -> AM57XX_INDETHER_LED1_GRN
    GPIO1_OE_ptr->OUTPUTEN_5 = 0x0;     //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)

    GPIO7_OE_ptr->OUTPUTEN_8 = 0x0;     //IDK_572x -> AM57XX_INDETHER_LED2_GRN
    GPIO7_OE_ptr->OUTPUTEN_9 = 0x0;     //IDK_572x -> AM57XX_INDETHER_LED2_RED
    GPIO7_OE_ptr->OUTPUTEN_10 = 0x0;    //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)

    GPIO3_OE_ptr->OUTPUTEN_17 = 0x0;    //IDK_572x -> AM57XX_INDETHER_LED3_GRN
    GPIO3_OE_ptr->OUTPUTEN_18 = 0x0;    //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
    GPIO7_OE_ptr->OUTPUTEN_11 = 0x0;    //IDK_572x -> AM57XX_INDETHER_LED3_RED

    /* Switch multiplexor */
    CTRL_CORE_PAD_VIN1A_D7_ptr     ->MUXMODE = VIN1A_D7_MUXMODE_gpio3_11   ; //IDK_572x -> AM57XX_STATUSLED0_GRN
    CTRL_CORE_PAD_VIN1A_D8_ptr     ->MUXMODE = VIN1A_D8_MUXMODE_gpio3_12   ; //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
    CTRL_CORE_PAD_VIN2A_VSYNC0_ptr ->MUXMODE = VIN2A_VSYNC0_MUXMODE_gpio4_0; //IDK_572x -> AM57XX_STATUSLED0_RED

    CTRL_CORE_PAD_VIN1A_D6_ptr     ->MUXMODE = VIN1A_D6_MUXMODE_gpio3_10   ; //IDK_572x -> AM57XX_STATUSLED1_RED
    CTRL_CORE_PAD_UART1_RXD_ptr    ->MUXMODE = UART1_RXD_MUXMODE_gpio7_22  ; //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
    CTRL_CORE_PAD_UART1_TXD_ptr    ->MUXMODE = UART1_TXD_MUXMODE_gpio7_23  ; //IDK_572x -> AM57XX_STATUSLED1_GRN

    CTRL_CORE_PAD_VIN1A_D5_ptr     ->MUXMODE = VIN1A_D5_MUXMODE_gpio3_9    ; //IDK_572x -> AM57XX_INDETHER_LED0_GRN
    CTRL_CORE_PAD_XREF_CLK2_ptr    ->MUXMODE = XREF_CLK2_MUXMODE_gpio6_19  ; //IDK_572x -> AM57XX_INDETHER_LED0_RED
    CTRL_CORE_PAD_MCASP2_AXR4_ptr  ->MUXMODE = MCASP2_AXR4_MUXMODE_gpio1_4 ; //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0

    CTRL_CORE_PAD_MCASP2_AXR5_ptr  ->MUXMODE = MCASP2_AXR5_MUXMODE_gpio6_7 ; //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    CTRL_CORE_PAD_MCASP2_AXR6_ptr  ->MUXMODE = MCASP2_AXR6_MUXMODE_gpio2_29; //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    CTRL_CORE_PAD_MCASP2_AXR7_ptr  ->MUXMODE = MCASP2_AXR7_MUXMODE_gpio1_5 ; //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

    CTRL_CORE_PAD_SPI1_D1_ptr      ->MUXMODE = SPI1_D1_MUXMODE_gpio7_8     ; //IDK_572x -> AM57XX_INDETHER_LED2_GRN
    CTRL_CORE_PAD_SPI1_D0_ptr      ->MUXMODE = SPI1_D0_MUXMODE_gpio7_9     ; //IDK_572x -> AM57XX_INDETHER_LED2_RED
    CTRL_CORE_PAD_SPI1_CS0_ptr     ->MUXMODE = SPI1_CS0_MUXMODE_gpio7_10   ; //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)

    CTRL_CORE_PAD_VIN1A_D13_ptr    ->MUXMODE = VIN1A_D13_MUXMODE_gpio3_17  ; //IDK_572x -> AM57XX_INDETHER_LED3_GRN
    CTRL_CORE_PAD_VIN1A_D14_ptr    ->MUXMODE = VIN1A_D14_MUXMODE_gpio3_18  ; //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
    CTRL_CORE_PAD_SPI1_CS1_ptr     ->MUXMODE = SPI1_CS1_MUXMODE_gpio7_11   ; //IDK_572x -> AM57XX_INDETHER_LED3_RED

    /* 0 == Enable */
    CTRL_CORE_PAD_VIN1A_D7_ptr     ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED0_GRN
    CTRL_CORE_PAD_VIN1A_D8_ptr     ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
    CTRL_CORE_PAD_VIN2A_VSYNC0_ptr ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED0_RED

    CTRL_CORE_PAD_VIN1A_D6_ptr     ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED1_RED
    CTRL_CORE_PAD_UART1_RXD_ptr    ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
    CTRL_CORE_PAD_UART1_TXD_ptr    ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED1_GRN

    CTRL_CORE_PAD_MCASP2_AXR4_ptr  ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
    CTRL_CORE_PAD_VIN1A_D5_ptr     ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED0_GRN
    CTRL_CORE_PAD_XREF_CLK2_ptr    ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED0_RED

    CTRL_CORE_PAD_MCASP2_AXR5_ptr  ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    CTRL_CORE_PAD_MCASP2_AXR6_ptr  ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    CTRL_CORE_PAD_MCASP2_AXR7_ptr  ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

    CTRL_CORE_PAD_SPI1_D1_ptr      ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED2_GRN
    CTRL_CORE_PAD_SPI1_D0_ptr      ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED2_RED
    CTRL_CORE_PAD_SPI1_CS0_ptr     ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)

    CTRL_CORE_PAD_VIN1A_D13_ptr    ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED3_GRN
    CTRL_CORE_PAD_VIN1A_D14_ptr    ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
    CTRL_CORE_PAD_SPI1_CS1_ptr     ->PULLUDENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED3_RED

    /* 0 == Pull down */
    CTRL_CORE_PAD_VIN1A_D7_ptr     ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_STATUSLED0_GRN
    CTRL_CORE_PAD_VIN1A_D8_ptr     ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
    CTRL_CORE_PAD_VIN2A_VSYNC0_ptr ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_STATUSLED0_RED

    CTRL_CORE_PAD_VIN1A_D6_ptr     ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_STATUSLED1_RED
    CTRL_CORE_PAD_UART1_RXD_ptr    ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
    CTRL_CORE_PAD_UART1_TXD_ptr    ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_STATUSLED1_GRN

    CTRL_CORE_PAD_MCASP2_AXR4_ptr  ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)    mcasp8_axr0
    CTRL_CORE_PAD_VIN1A_D5_ptr     ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED0_GRN
    CTRL_CORE_PAD_XREF_CLK2_ptr    ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED0_RED

    CTRL_CORE_PAD_MCASP2_AXR5_ptr  ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_RED          mcasp8_axr1
    CTRL_CORE_PAD_MCASP2_AXR6_ptr  ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_GRN          mcasp8_aclkx    mcasp8_aclkr
    CTRL_CORE_PAD_MCASP2_AXR7_ptr  ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)    mcasp8_fsx      mcasp8_fsr

    CTRL_CORE_PAD_SPI1_D1_ptr      ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED2_GRN
    CTRL_CORE_PAD_SPI1_D0_ptr      ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED2_RED
    CTRL_CORE_PAD_SPI1_CS0_ptr     ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)

    CTRL_CORE_PAD_VIN1A_D13_ptr    ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED3_GRN
    CTRL_CORE_PAD_VIN1A_D14_ptr    ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
    CTRL_CORE_PAD_SPI1_CS1_ptr     ->PULLTYPESELECT = 0x0; //IDK_572x -> AM57XX_INDETHER_LED3_RED

    /* 0 == Receive disabled */
    CTRL_CORE_PAD_VIN1A_D7_ptr     ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED0_GRN
    CTRL_CORE_PAD_VIN1A_D8_ptr     ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
    CTRL_CORE_PAD_VIN2A_VSYNC0_ptr ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED0_RED

    CTRL_CORE_PAD_VIN1A_D6_ptr     ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED1_RED
    CTRL_CORE_PAD_UART1_RXD_ptr    ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
    CTRL_CORE_PAD_UART1_TXD_ptr    ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_STATUSLED1_GRN

    CTRL_CORE_PAD_MCASP2_AXR4_ptr  ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)
    CTRL_CORE_PAD_VIN1A_D5_ptr     ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED0_GRN
    CTRL_CORE_PAD_XREF_CLK2_ptr    ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED0_RED

    CTRL_CORE_PAD_MCASP2_AXR5_ptr  ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_RED
    CTRL_CORE_PAD_MCASP2_AXR6_ptr  ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_GRN
    CTRL_CORE_PAD_MCASP2_AXR7_ptr  ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)

    CTRL_CORE_PAD_SPI1_D1_ptr      ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED2_GRN
    CTRL_CORE_PAD_SPI1_D0_ptr      ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED2_RED
    CTRL_CORE_PAD_SPI1_CS0_ptr     ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)

    CTRL_CORE_PAD_VIN1A_D13_ptr    ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED3_GRN
    CTRL_CORE_PAD_VIN1A_D14_ptr    ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
    CTRL_CORE_PAD_SPI1_CS1_ptr     ->INPUTENABLE = 0x0; //IDK_572x -> AM57XX_INDETHER_LED3_RED
#if (DEBUG_LEDS_IDK_IDK_AM5728)
    set_status_0_red ();
    set_status_1_red ();
    set_industial_2_red ();
    set_industial_3_red ();
    __delay_cycles(100000000);
#endif
    /* All off */
    GPIO3_DATAOUT_ptr->GPIO_11  = 0x0;  //IDK_572x -> AM57XX_STATUSLED0_GRN
    GPIO3_DATAOUT_ptr->GPIO_12  = 0x0;  //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
    GPIO4_DATAOUT_ptr->GPIO_0   = 0x0;  //IDK_572x -> AM57XX_STATUSLED0_RED

    GPIO3_DATAOUT_ptr->GPIO_10  = 0x0;  //IDK_572x -> AM57XX_STATUSLED1_RED
    GPIO7_DATAOUT_ptr->GPIO_22  = 0x0;  //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
    GPIO7_DATAOUT_ptr->GPIO_23  = 0x0;  //IDK_572x -> AM57XX_STATUSLED1_GRN

    GPIO1_DATAOUT_ptr->GPIO_4   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED0_YEL(blue)
    GPIO3_DATAOUT_ptr->GPIO_9   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED0_GRN
    GPIO6_DATAOUT_ptr->GPIO_19  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED0_RED

    GPIO6_DATAOUT_ptr->GPIO_7   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_RED
    GPIO2_DATAOUT_ptr->GPIO_29  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_GRN
    GPIO1_DATAOUT_ptr->GPIO_5   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED1_YEL(blue)

    GPIO7_DATAOUT_ptr->GPIO_8   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED2_GRN
    GPIO7_DATAOUT_ptr->GPIO_9   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED2_RED
    GPIO7_DATAOUT_ptr->GPIO_10  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)

    GPIO3_DATAOUT_ptr->GPIO_17  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED3_GRN
    GPIO3_DATAOUT_ptr->GPIO_18  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
    GPIO7_DATAOUT_ptr->GPIO_11  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED3_RED
#if (DEBUG_LEDS_IDK_IDK_AM5728)
    set_status_0_green ();
    set_status_1_green ();
    set_industial_2_green ();
    set_industial_3_green ();
#endif

}

void set_status_0_red (void){
    GPIO3_DATAOUT_ptr->GPIO_11  = 0x0;  //IDK_572x -> AM57XX_STATUSLED0_GRN
    GPIO3_DATAOUT_ptr->GPIO_12  = 0x0;  //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
    GPIO4_DATAOUT_ptr->GPIO_0   = 0x1;  //IDK_572x -> AM57XX_STATUSLED0_RED
}

void set_status_1_red (void){
    GPIO3_DATAOUT_ptr->GPIO_10  = 0x1;  //IDK_572x -> AM57XX_STATUSLED1_RED
    GPIO7_DATAOUT_ptr->GPIO_22  = 0x0;  //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
    GPIO7_DATAOUT_ptr->GPIO_23  = 0x0;  //IDK_572x -> AM57XX_STATUSLED1_GRN
}

void set_industial_2_red (void){
    GPIO7_DATAOUT_ptr->GPIO_8   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED2_GRN
    GPIO7_DATAOUT_ptr->GPIO_9   = 0x1;  //IDK_572x -> AM57XX_INDETHER_LED2_RED
    GPIO7_DATAOUT_ptr->GPIO_10  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)
}

void set_industial_3_red (void){
    GPIO3_DATAOUT_ptr->GPIO_17  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED3_GRN
    GPIO3_DATAOUT_ptr->GPIO_18  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
    GPIO7_DATAOUT_ptr->GPIO_11  = 0x1;  //IDK_572x -> AM57XX_INDETHER_LED3_RED
}

void set_status_0_green (void){
    GPIO3_DATAOUT_ptr->GPIO_11  = 0x1;  //IDK_572x -> AM57XX_STATUSLED0_GRN
    GPIO3_DATAOUT_ptr->GPIO_12  = 0x0;  //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
    GPIO4_DATAOUT_ptr->GPIO_0   = 0x0;  //IDK_572x -> AM57XX_STATUSLED0_RED
}

void set_status_1_green (void){
    GPIO3_DATAOUT_ptr->GPIO_10  = 0x0;  //IDK_572x -> AM57XX_STATUSLED1_RED
    GPIO7_DATAOUT_ptr->GPIO_22  = 0x0;  //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
    GPIO7_DATAOUT_ptr->GPIO_23  = 0x1;  //IDK_572x -> AM57XX_STATUSLED1_GRN
}

void set_industial_2_green (void){
    GPIO7_DATAOUT_ptr->GPIO_8   = 0x1;  //IDK_572x -> AM57XX_INDETHER_LED2_GRN
    GPIO7_DATAOUT_ptr->GPIO_9   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED2_RED
    GPIO7_DATAOUT_ptr->GPIO_10  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)
}

void set_industial_3_green (void){
    GPIO3_DATAOUT_ptr->GPIO_17  = 0x1;  //IDK_572x -> AM57XX_INDETHER_LED3_GRN
    GPIO3_DATAOUT_ptr->GPIO_18  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
    GPIO7_DATAOUT_ptr->GPIO_11  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED3_RED
}

void set_status_0_blue (void){
    GPIO3_DATAOUT_ptr->GPIO_11  = 0x0;  //IDK_572x -> AM57XX_STATUSLED0_GRN
    GPIO3_DATAOUT_ptr->GPIO_12  = 0x1;  //IDK_572x -> AM57XX_STATUSLED0_YEL(blue)
    GPIO4_DATAOUT_ptr->GPIO_0   = 0x0;  //IDK_572x -> AM57XX_STATUSLED0_RED
}

void set_status_1_blue (void){
    GPIO3_DATAOUT_ptr->GPIO_10  = 0x0;  //IDK_572x -> AM57XX_STATUSLED1_RED
    GPIO7_DATAOUT_ptr->GPIO_22  = 0x1;  //IDK_572x -> AM57XX_STATUSLED1_YEL(blue)
    GPIO7_DATAOUT_ptr->GPIO_23  = 0x1;  //IDK_572x -> AM57XX_STATUSLED1_GRN
}

void set_industial_2_blue (void){
    GPIO7_DATAOUT_ptr->GPIO_8   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED2_GRN
    GPIO7_DATAOUT_ptr->GPIO_9   = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED2_RED
    GPIO7_DATAOUT_ptr->GPIO_10  = 0x1;  //IDK_572x -> AM57XX_INDETHER_LED2_YEL(blue)
}

void set_industial_3_blue (void){
    GPIO3_DATAOUT_ptr->GPIO_17  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED3_GRN
    GPIO3_DATAOUT_ptr->GPIO_18  = 0x1;  //IDK_572x -> AM57XX_INDETHER_LED3_YEL(blue)
    GPIO7_DATAOUT_ptr->GPIO_11  = 0x0;  //IDK_572x -> AM57XX_INDETHER_LED3_RED
}

#endif /* DEBUG_LEDS_IDK_IDK_AM5728 */
