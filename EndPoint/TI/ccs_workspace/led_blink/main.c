#include "main.h"

#define CONTROL_MODULE_ADDR                     0x44E10000
#define CONTROL_SYSCONFIG_OFF                   0x10
#define CONTROL_DEV_ID_OFF                      0x600
uint32_t * CONTROL_SYSCONFIG_PTR                = (uint32_t *)( CONTROL_MODULE_ADDR + CONTROL_SYSCONFIG_OFF );
#define FREEEMU_DIS                             (uint32_t)(0b1 << 1)
#define IDLEMODE_NO_IDLE                        (uint32_t)(0b01 << 2)
#define STANDBY_NO_STANDBY                      (uint32_t)(0b01 << 4)
/* -------------------------------------------------------------------------- */
                                                        //MCASP PIN     MODE    PIN NAME        GPIO
#define CONF_LCD_DATA8_OFF                      0x8C0   //MCASP0_ACLKX  MODE3   LCD_DATA8       gpio2_14
#define CONF_LCD_DATA9_OFF                      0x8C4   //MCASP0_FSX    MODE3   LCD_DATA9       gpio2_15
#define CONF_LCD_DATA10_OFF                     0x8C8   //MCASP0_AXR0   MODE3   LCD_DATA10      gpio2_16
#define CONF_LCD_DATA14_OFF                     0x8D8   //MCASP0_AXR1   MODE3   LCD_DATA14      gpio0_10
                                                        //MCASP PIN     MODE    PIN NAME        GPIO
#define CONF_MCASP0_ACLKR_OFF                   0x9A0   //MCASP1_ACLKX  MODE3   MCASP0_ACLKR    gpio3_18
#define CONF_MCASP0_FSR_OFF                     0x9A4   //MCASP1_FSX    MODE3   MCASP0_FSR      gpio3_21
#define CONF_MCASP0_AXR1_OFF                    0x9A8   //MCASP1_AXR0   MODE3   MCASP0_AXR      gpio3_19
#define CONF_MCASP0_AHCLKX_OFF                  0x9AC   //MCASP1_AXR1   MODE3   MCASP0_AHCLKX   gpio3_20
                                                        //BBB   Pad    Pad name
#define CONF_GPMC_A5_OFF                        0x854   //USR0  V15    GPMC_A5  GPIO1_21
#define CONF_GPMC_A6_OFF                        0x858   //USR1  U15    GPMC_A6  GPIO1_22
#define CONF_GPMC_A7_OFF                        0x85C   //USR2  T15    GPMC_A7  GPIO1_23
#define CONF_GPMC_A8_OFF                        0x860   //USR3  V16    GPMC_A8  GPIO1_24

volatile uint32_t * CONF_LCD_DATA8_PTR                   = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_LCD_DATA8_OFF);       //MCASP0_ACLKX  MODE3
volatile uint32_t * CONF_LCD_DATA9_PTR                   = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_LCD_DATA9_OFF);       //MCASP0_FSX    MODE3
volatile uint32_t * CONF_LCD_DATA10_PTR                  = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_LCD_DATA10_OFF);      //MCASP0_AXR0   MODE3
volatile uint32_t * CONF_LCD_DATA14_PTR                  = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_LCD_DATA14_OFF);      //MCASP0_AXR1   MODE3

volatile uint32_t * CONF_MCASP0_ACLKR_PTR                = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_MCASP0_ACLKR_OFF);    //MCASP1_ACLKX  MODE3
volatile uint32_t * CONF_MCASP0_FSR_PTR                  = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_MCASP0_FSR_OFF);      //MCASP1_FSX    MODE3
volatile uint32_t * CONF_MCASP0_AXR1_PTR                 = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_MCASP0_AXR1_OFF);     //MCASP1_AXR0   MODE3
volatile uint32_t * CONF_MCASP0_AHCLKX_PTR               = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_MCASP0_AHCLKX_OFF);   //MCASP1_AXR1   MODE3

volatile uint32_t * CONF_GPMC_A5_PTR                     = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_GPMC_A5_OFF);         //USR0   V15    GPMC_A5
volatile uint32_t * CONF_GPMC_A6_PTR                     = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_GPMC_A6_OFF);         //USR1   U15    GPMC_A6
volatile uint32_t * CONF_GPMC_A7_PTR                     = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_GPMC_A7_OFF);         //USR2   T15    GPMC_A7
volatile uint32_t * CONF_GPMC_A8_PTR                     = (uint32_t *)(CONTROL_MODULE_ADDR + CONF_GPMC_A8_OFF);         //USR3   V16    GPMC_A8

volatile struct CONF_MODULE_PIN * CONF_LCD_DATA8         = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_LCD_DATA8_OFF);       //MCASP0_ACLKX  MODE3
volatile struct CONF_MODULE_PIN * CONF_LCD_DATA9         = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_LCD_DATA9_OFF);       //MCASP0_FSX    MODE3
volatile struct CONF_MODULE_PIN * CONF_LCD_DATA10        = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_LCD_DATA10_OFF);      //MCASP0_AXR0   MODE3
volatile struct CONF_MODULE_PIN * CONF_LCD_DATA14        = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_LCD_DATA14_OFF);      //MCASP0_AXR1   MODE3

volatile struct CONF_MODULE_PIN * CONF_MCASP0_ACLKR      = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_MCASP0_ACLKR_OFF);    //MCASP1_ACLKX  MODE3
volatile struct CONF_MODULE_PIN * CONF_MCASP0_FSR        = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_MCASP0_FSR_OFF);      //MCASP1_FSX    MODE3
volatile struct CONF_MODULE_PIN * CONF_MCASP0_AXR1       = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_MCASP0_AXR1_OFF);     //MCASP1_AXR0   MODE3
volatile struct CONF_MODULE_PIN * CONF_MCASP0_AHCLKX     = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_MCASP0_AHCLKX_OFF);   //MCASP1_AXR1   MODE3

volatile struct CONF_MODULE_PIN * CONF_GPMC_A5           = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_GPMC_A5_OFF);         //USR0    V15   GPMC_A5  GPIO1_21
volatile struct CONF_MODULE_PIN * CONF_GPMC_A6           = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_GPMC_A6_OFF);         //USR1    U15   GPMC_A6  GPIO1_22
volatile struct CONF_MODULE_PIN * CONF_GPMC_A7           = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_GPMC_A7_OFF);         //USR2    T15   GPMC_A7  GPIO1_23
volatile struct CONF_MODULE_PIN * CONF_GPMC_A8           = (struct CONF_MODULE_PIN *)(CONTROL_MODULE_ADDR + CONF_GPMC_A8_OFF);         //USR3    V16   GPMC_A8  GPIO1_24


/* Pad functional signal mux select. */
#define MODE0                                   0b000
#define MODE1                                   0b001
#define MODE2                                   0b010
#define MODE3                                   0b011
#define MODE4                                   0b100
#define MODE5                                   0b101
#define MODE6                                   0b110
#define MODE7                                   0b111 /* GPIO */
#define CONF_MODULE_PIN_MODE_VAL(x)             (x & 0b111)

/* Pad pullup/pulldown enable. */
#define PULL_EN                                 0
#define PULL_DIS                                1
#define CONF_MODULE_PIN_MODE_PUDEN_VAL(x)       (x << 3)

/* Pad pullup/pulldown type selection. */
#define PULLDOWN_SEL                            0
#define PULLUP_SEL                              1
#define CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(x)   (x << 4)

/* Input enable value for the PAD */
#define RX_ACTIVE_DIS                           0
#define RX_ACTIVE_EN                            1
#define CONF_MODULE_PIN_RX_ACTIVE_VAL(x)        (x << 5)

/* Select between faster or slower slew rate */
#define FAST_SLEW                               0
#define SLOW_SLEW                               1
#define CONF_MODULE_PIN_SLEWCTL_VAL(x)          (x << 6)

/* -------------------------------------------------------------------------- */
#define CM_WKUP_GPIO0_CLKCTRL_ADDR              0x44E00408
#define CM_PER_GPIO1_CLKCTRL_ADDR               0x44E000AC
#define CM_PER_GPIO2_CLKCTRL_ADDR               0x44E000B0
#define CM_PER_GPIO3_CLKCTRL_ADDR               0x44E000B4


volatile uint32_t                   * CM_WKUP_GPIO0_CLKCTRL_PTR = (uint32_t *)( CM_WKUP_GPIO0_CLKCTRL_ADDR );
volatile uint32_t                   * CM_PER_GPIO1_CLKCTRL_PTR  = (uint32_t *)( CM_PER_GPIO1_CLKCTRL_ADDR );
volatile uint32_t                   * CM_PER_GPIO2_CLKCTRL_PTR  = (uint32_t *)( CM_PER_GPIO2_CLKCTRL_ADDR );
volatile uint32_t                   * CM_PER_GPIO3_CLKCTRL_PTR  = (uint32_t *)( CM_PER_GPIO3_CLKCTRL_ADDR );
#define MODULEMODE_ENABLE                                       0x2
#define OPTFCLKEN_GPIO_GDBCLK                                   (uint32_t)(1 << 18)
/* -------------------------------------------------------------------------- */

volatile struct GPIO_SYSCONFIG      * GPIO0_SYSCONFIG_ptr       = (struct GPIO_SYSCONFIG *)( GPIO0_REGS + GPIO_SYSCONFIG_OFF );
volatile struct GPIO_SYSCONFIG      * GPIO1_SYSCONFIG_ptr       = (struct GPIO_SYSCONFIG *)( GPIO1_REGS + GPIO_SYSCONFIG_OFF );
volatile struct GPIO_SYSCONFIG      * GPIO2_SYSCONFIG_ptr       = (struct GPIO_SYSCONFIG *)( GPIO2_REGS + GPIO_SYSCONFIG_OFF );
volatile struct GPIO_SYSCONFIG      * GPIO3_SYSCONFIG_ptr       = (struct GPIO_SYSCONFIG *)( GPIO3_REGS + GPIO_SYSCONFIG_OFF );

volatile struct GPIO_OE             * GPIO0_OE_ptr              = (struct GPIO_OE *)( GPIO0_REGS + GPIO_OE_OFF );
volatile struct GPIO_OE             * GPIO1_OE_ptr              = (struct GPIO_OE *)( GPIO1_REGS + GPIO_OE_OFF );
volatile struct GPIO_OE             * GPIO2_OE_ptr              = (struct GPIO_OE *)( GPIO2_REGS + GPIO_OE_OFF );
volatile struct GPIO_OE             * GPIO3_OE_ptr              = (struct GPIO_OE *)( GPIO3_REGS + GPIO_OE_OFF );

volatile struct GPIO_DATAOUT        * GPIO0_DATAOUT_ptr         = (struct GPIO_DATAOUT *)( GPIO0_REGS + GPIO_DATAOUT_OFF );
volatile struct GPIO_DATAOUT        * GPIO1_DATAOUT_ptr         = (struct GPIO_DATAOUT *)( GPIO1_REGS + GPIO_DATAOUT_OFF );
volatile struct GPIO_DATAOUT        * GPIO2_DATAOUT_ptr         = (struct GPIO_DATAOUT *)( GPIO2_REGS + GPIO_DATAOUT_OFF );
volatile struct GPIO_DATAOUT        * GPIO3_DATAOUT_ptr         = (struct GPIO_DATAOUT *)( GPIO3_REGS + GPIO_DATAOUT_OFF );

volatile struct GPIO_DATAOUT        * GPIO0_SETDATAOUT_ptr      = (struct GPIO_DATAOUT *)( GPIO0_REGS + GPIO_SETDATAOUT_OFF );
volatile struct GPIO_DATAOUT        * GPIO1_SETDATAOUT_ptr      = (struct GPIO_DATAOUT *)( GPIO1_REGS + GPIO_SETDATAOUT_OFF );
volatile struct GPIO_DATAOUT        * GPIO2_SETDATAOUT_ptr      = (struct GPIO_DATAOUT *)( GPIO2_REGS + GPIO_SETDATAOUT_OFF );
volatile struct GPIO_DATAOUT        * GPIO3_SETDATAOUT_ptr      = (struct GPIO_DATAOUT *)( GPIO3_REGS + GPIO_SETDATAOUT_OFF );

volatile struct GPIO_DATAOUT        * GPIO0_CLEARDATAOUT_ptr    = (struct GPIO_DATAOUT *)( GPIO0_REGS + GPIO_CLEARDATAOUT_OFF );
volatile struct GPIO_DATAOUT        * GPIO1_CLEARDATAOUT_ptr    = (struct GPIO_DATAOUT *)( GPIO1_REGS + GPIO_CLEARDATAOUT_OFF );
volatile struct GPIO_DATAOUT        * GPIO2_CLEARDATAOUT_ptr    = (struct GPIO_DATAOUT *)( GPIO2_REGS + GPIO_CLEARDATAOUT_OFF );
volatile struct GPIO_DATAOUT        * GPIO3_CLEARDATAOUT_ptr    = (struct GPIO_DATAOUT *)( GPIO3_REGS + GPIO_CLEARDATAOUT_OFF );
/* -------------------------------------------------------------------------- */
volatile uint32_t                   * GPIO0_SYSSTATUS           = (uint32_t *)( GPIO0_REGS + GPIO_SYSSTATUS_OFF );
volatile uint32_t                   * GPIO1_SYSSTATUS           = (uint32_t *)( GPIO1_REGS + GPIO_SYSSTATUS_OFF );
volatile uint32_t                   * GPIO2_SYSSTATUS           = (uint32_t *)( GPIO2_REGS + GPIO_SYSSTATUS_OFF );
volatile uint32_t                   * GPIO3_SYSSTATUS           = (uint32_t *)( GPIO3_REGS + GPIO_SYSSTATUS_OFF );

#define CM_WKUP_CONTROL_CLKCTRL_ADDR            0x44E00404
uint32_t * CM_WKUP_CONTROL_CLKCTRL_PTR          = (uint32_t *)( CM_WKUP_CONTROL_CLKCTRL_ADDR );
#if (SETUP_DPLL)
    #define CM_CLKSEL_DPLL_CORE_ADDR            0x44E00468
    #define DPLL_MULT(x)                        (uint32_t)((x & 0b11111111111) << 8)
    #define DPLL_DIV(x)                         (uint32_t)(x & 0b1111111)
    uint32_t * CM_CLKSEL_DPLL_CORE_PTR          = (uint32_t *)( CM_CLKSEL_DPLL_CORE_ADDR );

    #define CM_CLKMODE_DPLL_CORE_ADDR           0x44E00490
    #define DPLL_EN_MASK                        0b111
    #define DPLL_EN(x)                          (x & DPLL_EN_MASK)
    #define MN_BYPASS                           0b100
    uint32_t * CM_CLKMODE_DPLL_CORE_PTR         = (uint32_t *)( CM_CLKMODE_DPLL_CORE_ADDR );

    #define CM_DIV_M4_DPLL_CORE_ADDR            0x44E00480
    #define CM_DIV_M5_DPLL_CORE_ADDR            0x44E00484
    uint32_t * CM_DIV_M4_DPLL_CORE_PTR          = (uint32_t *)( CM_DIV_M4_DPLL_CORE_ADDR );
    uint32_t * CM_DIV_M5_DPLL_CORE_PTR          = (uint32_t *)( CM_DIV_M5_DPLL_CORE_ADDR );
    #define HSDIVIDER_CLKOUT_DIV(x)             (x & 0b11111)
    #define ST_HSDIVIDER_CLKOUT                 1<<9
#endif

int main(void)
{

#if (SETUP_DPLL)
    /* Put the PLL in bypass mode. */
    while ((* CM_CLKMODE_DPLL_CORE_PTR & DPLL_EN_MASK)!= MN_BYPASS)
        * CM_CLKMODE_DPLL_CORE_PTR = DPLL_EN(MN_BYPASS);
    /* Set the multiplier and divider values for the PLL. */
    * CM_CLKSEL_DPLL_CORE_PTR = DPLL_MULT(1000) | DPLL_DIV(23);
    /* Configure the High speed dividers */
    * CM_DIV_M4_DPLL_CORE_PTR = HSDIVIDER_CLKOUT_DIV(20);
    * CM_DIV_M5_DPLL_CORE_PTR = HSDIVIDER_CLKOUT_DIV(20);
    /* Now LOCK the PLL by enabling it */
    while ((* CM_CLKMODE_DPLL_CORE_PTR & DPLL_EN_MASK)!= DPLL_EN_MASK)
        * CM_CLKMODE_DPLL_CORE_PTR = DPLL_EN(DPLL_EN_MASK);
    * CM_DIV_M4_DPLL_CORE_PTR = (* CM_DIV_M4_DPLL_CORE_PTR) | ST_HSDIVIDER_CLKOUT;
    * CM_DIV_M5_DPLL_CORE_PTR = (* CM_DIV_M5_DPLL_CORE_PTR) | ST_HSDIVIDER_CLKOUT;
#endif
    * CM_WKUP_CONTROL_CLKCTRL_PTR   = MODULEMODE_ENABLE;

#if (0)
    * CONTROL_SYSCONFIG_PTR = FREEEMU_DIS | IDLEMODE_NO_IDLE | STANDBY_NO_STANDBY;
#endif

#if (0)
    * CM_WKUP_GPIO0_CLKCTRL_PTR = MODULEMODE_ENABLE | OPTFCLKEN_GPIO_GDBCLK;
    * CM_PER_GPIO1_CLKCTRL_PTR  = MODULEMODE_ENABLE | OPTFCLKEN_GPIO_GDBCLK;
    * CM_PER_GPIO2_CLKCTRL_PTR  = MODULEMODE_ENABLE | OPTFCLKEN_GPIO_GDBCLK;
    * CM_PER_GPIO3_CLKCTRL_PTR  = MODULEMODE_ENABLE | OPTFCLKEN_GPIO_GDBCLK;
#else
    * CM_WKUP_GPIO0_CLKCTRL_PTR = MODULEMODE_ENABLE;
    * CM_PER_GPIO1_CLKCTRL_PTR  = MODULEMODE_ENABLE;
    * CM_PER_GPIO2_CLKCTRL_PTR  = MODULEMODE_ENABLE;
    * CM_PER_GPIO3_CLKCTRL_PTR  = MODULEMODE_ENABLE;
#endif

#if (0)
    /* 1h = No-idle. An idle request is never acknowledged. */
    GPIO0_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO1_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO2_SYSCONFIG_ptr->IDLEMODE = 0x1;
    GPIO3_SYSCONFIG_ptr->IDLEMODE = 0x1;
#endif

#if (0)
    GPIO0_SYSCONFIG_ptr->SOFTRESET = 0x1;
    while (! * GPIO0_SYSSTATUS)
        ;
    GPIO0_SYSCONFIG_ptr->SOFTRESET = 0x0;

    GPIO1_SYSCONFIG_ptr->SOFTRESET = 0x1;
    while (! * GPIO1_SYSSTATUS)
        ;
    GPIO1_SYSCONFIG_ptr->SOFTRESET = 0x0;

    GPIO2_SYSCONFIG_ptr->SOFTRESET = 0x1;
    while (! * GPIO2_SYSSTATUS)
        ;
    GPIO2_SYSCONFIG_ptr->SOFTRESET = 0x0;

    GPIO3_SYSCONFIG_ptr->SOFTRESET = 0x1;
    while (! * GPIO3_SYSSTATUS)
        ;
    GPIO3_SYSCONFIG_ptr->SOFTRESET = 0x0;
#endif
    /* 0h = The corresponding GPIO port is configured as an output. */
                                            //MCASP PIN     MODE    PIN NAME        GPIO
    GPIO2_OE_ptr->OUTPUTEN_14   = 0x0;      //MCASP0_ACLKX  MODE3   LCD_DATA8       gpio2_14
    GPIO2_OE_ptr->OUTPUTEN_15   = 0x0;      //MCASP0_FSX    MODE3   LCD_DATA9       gpio2_15
    GPIO2_OE_ptr->OUTPUTEN_16   = 0x0;      //MCASP0_AXR0   MODE3   LCD_DATA10      gpio2_16
    GPIO0_OE_ptr->OUTPUTEN_10   = 0x0;      //MCASP0_AXR1   MODE3   LCD_DATA14      gpio0_10

    GPIO3_OE_ptr->OUTPUTEN_18   = 0x0;      //MCASP1_ACLKX  MODE3   MCASP0_ACLKR    gpio3_18
    GPIO3_OE_ptr->OUTPUTEN_21   = 0x0;      //MCASP1_FSX    MODE3   MCASP0_FSR      gpio3_21
    GPIO3_OE_ptr->OUTPUTEN_19   = 0x0;      //MCASP1_AXR0   MODE3   MCASP0_AXR      gpio3_19
    GPIO3_OE_ptr->OUTPUTEN_20   = 0x0;      //MCASP1_AXR1   MODE3   MCASP0_AHCLKX   gpio3_20

    GPIO1_OE_ptr->OUTPUTEN_21   = 0x0;       //USR0  V15  GPMC_A5  GPIO1_21
    GPIO1_OE_ptr->OUTPUTEN_22   = 0x0;       //USR1  U15  GPMC_A6  GPIO1_22
    GPIO1_OE_ptr->OUTPUTEN_23   = 0x0;       //USR2  T15  GPMC_A7  GPIO1_23
    GPIO1_OE_ptr->OUTPUTEN_24   = 0x0;       //USR3  V16  GPMC_A8  GPIO1_24

#if (1)
    * CONF_LCD_DATA8_PTR     = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //MCASP0_ACLKX  MODE3
    * CONF_LCD_DATA9_PTR     = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //MCASP0_FSX    MODE3 OUT
    * CONF_LCD_DATA10_PTR    = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //MCASP0_AXR0   MODE3 TX
    * CONF_LCD_DATA14_PTR    = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //MCASP0_AXR1   MODE3 RX
    * CONF_MCASP0_ACLKR_PTR  = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //MCASP1_ACLKX  MODE3
    * CONF_MCASP0_FSR_PTR    = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //MCASP1_FSX    MODE3
    * CONF_MCASP0_AXR1_PTR   = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //MCASP1_AXR0   MODE3 TX
    * CONF_MCASP0_AHCLKX_PTR = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //MCASP1_AXR1   MODE3 RX
    * CONF_GPMC_A5_PTR       = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //USR0  V15  GPMC_A5  GPIO1_21
    * CONF_GPMC_A6_PTR       = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //USR1  U15  GPMC_A6  GPIO1_22
    * CONF_GPMC_A7_PTR       = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //USR2  T15  GPMC_A7  GPIO1_23
    * CONF_GPMC_A8_PTR       = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);     //USR3  V16  GPMC_A8  GPIO1_24
#else                                           //MCASP PIN     MODE    PIN NAME        GPIO
    CONF_LCD_DATA8     ->MODE = MODE7;          //MCASP0_ACLKX  MODE3   LCD_DATA8       gpio2_14
    CONF_LCD_DATA9     ->MODE = MODE7;          //MCASP0_FSX    MODE3   LCD_DATA9       gpio2_15
    CONF_LCD_DATA10    ->MODE = MODE7;          //MCASP0_AXR0   MODE3   LCD_DATA10      gpio2_16
    CONF_LCD_DATA14    ->MODE = MODE7;          //MCASP0_AXR1   MODE3   LCD_DATA14      gpio0_10

    CONF_MCASP0_ACLKR  ->MODE = MODE7;          //MCASP1_ACLKX  MODE3   MCASP0_ACLKR    gpio3_18
    CONF_MCASP0_FSR    ->MODE = MODE7;          //MCASP1_FSX    MODE3   MCASP0_FSR      gpio3_21
    CONF_MCASP0_AXR1   ->MODE = MODE7;          //MCASP1_AXR0   MODE3   MCASP0_AXR      gpio3_19
    CONF_MCASP0_AHCLKX ->MODE = MODE7;          //MCASP1_AXR1   MODE3   MCASP0_AHCLKX   gpio3_20

    CONF_GPMC_A5       ->MODE = MODE7;          //USR0  V15  GPMC_A5  GPIO1_21
    CONF_GPMC_A6       ->MODE = MODE7;          //USR1  U15  GPMC_A6  GPIO1_22
    CONF_GPMC_A7       ->MODE = MODE7;          //USR2  T15  GPMC_A7  GPIO1_23
    CONF_GPMC_A8       ->MODE = MODE7;          //USR3  V16  GPMC_A8  GPIO1_24
/* -------------------------------------------------------------------------- */
    CONF_LCD_DATA8     ->PUDEN = 0x0;           //MCASP0_ACLKX  MODE3   LCD_DATA8       gpio2_14
    CONF_LCD_DATA9     ->PUDEN = 0x0;           //MCASP0_FSX    MODE3   LCD_DATA9       gpio2_15
    CONF_LCD_DATA10    ->PUDEN = 0x0;           //MCASP0_AXR0   MODE3   LCD_DATA10      gpio2_16
    CONF_LCD_DATA14    ->PUDEN = 0x0;           //MCASP0_AXR1   MODE3   LCD_DATA14      gpio0_10

    CONF_MCASP0_ACLKR  ->PUDEN = 0x0;           //MCASP1_ACLKX  MODE3   MCASP0_ACLKR    gpio3_18
    CONF_MCASP0_FSR    ->PUDEN = 0x0;           //MCASP1_FSX    MODE3   MCASP0_FSR      gpio3_21
    CONF_MCASP0_AXR1   ->PUDEN = 0x0;           //MCASP1_AXR0   MODE3   MCASP0_AXR      gpio3_19
    CONF_MCASP0_AHCLKX ->PUDEN = 0x0;           //MCASP1_AXR1   MODE3   MCASP0_AHCLKX   gpio3_20

    CONF_GPMC_A5       ->PUDEN = 0x0;           //USR0  V15  GPMC_A5  GPIO1_21
    CONF_GPMC_A6       ->PUDEN = 0x0;           //USR1  U15  GPMC_A6  GPIO1_22
    CONF_GPMC_A7       ->PUDEN = 0x0;           //USR2  T15  GPMC_A7  GPIO1_23
    CONF_GPMC_A8       ->PUDEN = 0x0;           //USR3  V16  GPMC_A8  GPIO1_24
/* -------------------------------------------------------------------------- */
    CONF_LCD_DATA8     ->PUTYPESEL = 0x1;       //MCASP0_ACLKX  MODE3   LCD_DATA8       gpio2_14
    CONF_LCD_DATA9     ->PUTYPESEL = 0x1;       //MCASP0_FSX    MODE3   LCD_DATA9       gpio2_15
    CONF_LCD_DATA10    ->PUTYPESEL = 0x1;       //MCASP0_AXR0   MODE3   LCD_DATA10      gpio2_16
    CONF_LCD_DATA14    ->PUTYPESEL = 0x1;       //MCASP0_AXR1   MODE3   LCD_DATA14      gpio0_10

    CONF_MCASP0_ACLKR  ->PUTYPESEL = 0x1;       //MCASP1_ACLKX  MODE3   MCASP0_ACLKR    gpio3_18
    CONF_MCASP0_FSR    ->PUTYPESEL = 0x1;       //MCASP1_FSX    MODE3   MCASP0_FSR      gpio3_21
    CONF_MCASP0_AXR1   ->PUTYPESEL = 0x1;       //MCASP1_AXR0   MODE3   MCASP0_AXR      gpio3_19
    CONF_MCASP0_AHCLKX ->PUTYPESEL = 0x1;       //MCASP1_AXR1   MODE3   MCASP0_AHCLKX   gpio3_20

    CONF_GPMC_A5       ->PUTYPESEL = 0x1;       //USR0  V15  GPMC_A5  GPIO1_21
    CONF_GPMC_A6       ->PUTYPESEL = 0x1;       //USR1  U15  GPMC_A6  GPIO1_22
    CONF_GPMC_A7       ->PUTYPESEL = 0x1;       //USR2  T15  GPMC_A7  GPIO1_23
    CONF_GPMC_A8       ->PUTYPESEL = 0x1;       //USR3  V16  GPMC_A8  GPIO1_24
/* -------------------------------------------------------------------------- */
#if (0)
    CONF_LCD_DATA8     ->RXACTIVE = 0x1;        //MCASP0_ACLKX  MODE3   LCD_DATA8       gpio2_14
    CONF_LCD_DATA9     ->RXACTIVE = 0x1;        //MCASP0_FSX    MODE3   LCD_DATA9       gpio2_15
    CONF_LCD_DATA10    ->RXACTIVE = 0x1;        //MCASP0_AXR0   MODE3   LCD_DATA10      gpio2_16
    CONF_LCD_DATA14    ->RXACTIVE = 0x1;        //MCASP0_AXR1   MODE3   LCD_DATA14      gpio0_10

    CONF_MCASP0_ACLKR  ->RXACTIVE = 0x1;        //MCASP1_ACLKX  MODE3   MCASP0_ACLKR    gpio3_18
    CONF_MCASP0_FSR    ->RXACTIVE = 0x1;        //MCASP1_FSX    MODE3   MCASP0_FSR      gpio3_21
    CONF_MCASP0_AXR1   ->RXACTIVE = 0x1;        //MCASP1_AXR0   MODE3   MCASP0_AXR      gpio3_19
    CONF_MCASP0_AHCLKX ->RXACTIVE = 0x1;        //MCASP1_AXR1   MODE3   MCASP0_AHCLKX   gpio3_20

    CONF_GPMC_A5       ->RXACTIVE = 0x1;        //USR0  V15  GPMC_A5  GPIO1_21
    CONF_GPMC_A6       ->RXACTIVE = 0x1;        //USR1  U15  GPMC_A6  GPIO1_22
    CONF_GPMC_A7       ->RXACTIVE = 0x1;        //USR2  T15  GPMC_A7  GPIO1_23
    CONF_GPMC_A8       ->RXACTIVE = 0x1;        //USR3  V16  GPMC_A8  GPIO1_24
#endif
#endif

#if (1)
    uint32_t CONTROL_rev            = * (uint32_t *)( CONTROL_MODULE_ADDR );
    uint32_t CONF_LCD_DATA8_val     = * CONF_LCD_DATA8_PTR;
    uint32_t CONF_LCD_DATA9_val     = * CONF_LCD_DATA9_PTR;
    uint32_t CONF_LCD_DATA10_val    = * CONF_LCD_DATA10_PTR;
    uint32_t CONF_LCD_DATA14_val    = * CONF_LCD_DATA14_PTR;

    uint32_t CONF_MCASP0_ACLKR_val  = * CONF_MCASP0_ACLKR_PTR;
    uint32_t CONF_MCASP0_FSR_val    = * CONF_MCASP0_FSR_PTR;
    uint32_t CONF_MCASP0_AXR1_val   = * CONF_MCASP0_AXR1_PTR;
    uint32_t CONF_MCASP0_AHCLKX_val = * CONF_MCASP0_AHCLKX_PTR;

    uint32_t DEV_ID_val             = * (uint32_t *)(CONTROL_MODULE_ADDR + CONTROL_DEV_ID_OFF);
    uint32_t GPIO0_rev_val          = * (uint32_t *)(GPIO0_REGS);
    uint32_t GPIO1_rev_val          = * (uint32_t *)(GPIO1_REGS);
    uint32_t GPIO2_rev_val          = * (uint32_t *)(GPIO2_REGS);
    uint32_t GPIO3_rev_val          = * (uint32_t *)(GPIO3_REGS);
#endif
    while (1) {
        /* McASP0 pins */                        //MCASP PIN     MODE    PIN NAME        GPIO
        GPIO2_DATAOUT_ptr     ->GPIO_14  = 0x1;  //MCASP0_ACLKX  MODE3   LCD_DATA8       gpio2_14
        GPIO2_DATAOUT_ptr     ->GPIO_15  = 0x1;  //MCASP0_FSX    MODE3   LCD_DATA9       gpio2_15
        GPIO2_DATAOUT_ptr     ->GPIO_16  = 0x1;  //MCASP0_AXR0   MODE3   LCD_DATA10      gpio2_16
        GPIO0_DATAOUT_ptr     ->GPIO_10  = 0x1;  //MCASP0_AXR1   MODE3   LCD_DATA14      gpio0_10
        /* McASP1 pins */
        GPIO3_DATAOUT_ptr     ->GPIO_18  = 0x1;  //MCASP1_ACLKX  MODE3   MCASP0_ACLKR    gpio3_18
        GPIO3_DATAOUT_ptr     ->GPIO_21  = 0x1;  //MCASP1_FSX    MODE3   MCASP0_FSR      gpio3_21
        GPIO3_DATAOUT_ptr     ->GPIO_19  = 0x1;  //MCASP1_AXR0   MODE3   MCASP0_AXR      gpio3_19
        GPIO3_DATAOUT_ptr     ->GPIO_20  = 0x1;  //MCASP1_AXR1   MODE3   MCASP0_AHCLKX   gpio3_20
        /* LEDs */
        GPIO1_SETDATAOUT_ptr  ->GPIO_21  = 0x1;  //USR0  V15  GPMC_A5  GPIO1_21
        GPIO1_SETDATAOUT_ptr  ->GPIO_22  = 0x1;  //USR1  U15  GPMC_A6  GPIO1_22
        GPIO1_SETDATAOUT_ptr  ->GPIO_23  = 0x1;  //USR2  T15  GPMC_A7  GPIO1_23
        GPIO1_SETDATAOUT_ptr  ->GPIO_24  = 0x1;  //USR3  V16  GPMC_A8  GPIO1_24

        /* McASP0 pins */                        //MCASP PIN     MODE    PIN NAME        GPIO
        GPIO2_DATAOUT_ptr     ->GPIO_14  = 0x0;  //MCASP0_ACLKX  MODE3   LCD_DATA8       gpio2_14
        GPIO2_DATAOUT_ptr     ->GPIO_15  = 0x0;  //MCASP0_FSX    MODE3   LCD_DATA9       gpio2_15
        GPIO2_DATAOUT_ptr     ->GPIO_16  = 0x0;  //MCASP0_AXR0   MODE3   LCD_DATA10      gpio2_16
        GPIO0_DATAOUT_ptr     ->GPIO_10  = 0x0;  //MCASP0_AXR1   MODE3   LCD_DATA14      gpio0_10
        /* McASP1 pins */
        GPIO3_DATAOUT_ptr     ->GPIO_18  = 0x0;  //MCASP1_ACLKX  MODE3   MCASP0_ACLKR    gpio3_18
        GPIO3_DATAOUT_ptr     ->GPIO_21  = 0x0;  //MCASP1_FSX    MODE3   MCASP0_FSR      gpio3_21
        GPIO3_DATAOUT_ptr     ->GPIO_19  = 0x0;  //MCASP1_AXR0   MODE3   MCASP0_AXR      gpio3_19
        GPIO3_DATAOUT_ptr     ->GPIO_20  = 0x0;  //MCASP1_AXR1   MODE3   MCASP0_AHCLKX   gpio3_20
        /* LEDs */
        GPIO1_CLEARDATAOUT_ptr->GPIO_21  = 0x1;  //USR0  V15  GPMC_A5  GPIO1_21
        GPIO1_CLEARDATAOUT_ptr->GPIO_22  = 0x1;  //USR1  U15  GPMC_A6  GPIO1_22
        GPIO1_CLEARDATAOUT_ptr->GPIO_23  = 0x1;  //USR2  T15  GPMC_A7  GPIO1_23
        GPIO1_CLEARDATAOUT_ptr->GPIO_24  = 0x1;  //USR3  V16  GPMC_A8  GPIO1_24

    } //end while (1)
}
