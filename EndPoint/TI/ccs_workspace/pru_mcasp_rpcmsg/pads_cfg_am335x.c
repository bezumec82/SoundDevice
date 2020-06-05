#include "pads_cfg.h"
#if (RUN_IN_ARM) /* The control module includes status and control logic not addressed within the peripherals or the rest of the device infrastructure. */
#define CONTROL_MODULE_ADDR                     0x44E10000

#define CONF_LCD_DATA8_OFF                      0x8C0   //MCASP0_ACLKX  MODE3
#define CONF_LCD_DATA9_OFF                      0x8C4   //MCASP0_FSX    MODE3
#define CONF_LCD_DATA10_OFF                     0x8C8   //MCASP0_AXR0   MODE3
#define CONF_LCD_DATA14_OFF                     0x8D8   //MCASP0_AXR1   MODE3
volatile uint32_t * CONF_LCD_DATA8_PTR          = (uint32_t *)( CONTROL_MODULE_ADDR + CONF_LCD_DATA8_OFF );     //MCASP0_ACLKX  MODE3
volatile uint32_t * CONF_LCD_DATA9_PTR          = (uint32_t *)( CONTROL_MODULE_ADDR + CONF_LCD_DATA9_OFF );     //MCASP0_FSX    MODE3
volatile uint32_t * CONF_LCD_DATA10_PTR         = (uint32_t *)( CONTROL_MODULE_ADDR + CONF_LCD_DATA10_OFF );    //MCASP0_AXR0   MODE3
volatile uint32_t * CONF_LCD_DATA14_PTR         = (uint32_t *)( CONTROL_MODULE_ADDR + CONF_LCD_DATA14_OFF );    //MCASP0_AXR1   MODE3

#define CONF_MCASP0_ACLKR_OFF                   0x9A0   //MCASP1_ACLKX  MODE3
#define CONF_MCASP0_FSR_OFF                     0x9A4   //MCASP1_FSX    MODE3
#define CONF_MCASP0_AXR1_OFF                    0x9A8   //MCASP1_AXR0   MODE3
#define CONF_MCASP0_AHCLKX_OFF                  0x9AC   //MCASP1_AXR1   MODE3
volatile uint32_t * CONF_MCASP0_ACLKR_PTR       = (uint32_t *)( CONTROL_MODULE_ADDR + CONF_MCASP0_ACLKR_OFF );  //MCASP1_ACLKX  MODE3
volatile uint32_t * CONF_MCASP0_FSR_PTR         = (uint32_t *)( CONTROL_MODULE_ADDR + CONF_MCASP0_FSR_OFF );    //MCASP1_FSX    MODE3
volatile uint32_t * CONF_MCASP0_AXR1_PTR        = (uint32_t *)( CONTROL_MODULE_ADDR + CONF_MCASP0_AXR1_OFF );   //MCASP1_AXR0   MODE3
volatile uint32_t * CONF_MCASP0_AHCLKX_PTR      = (uint32_t *)( CONTROL_MODULE_ADDR + CONF_MCASP0_AHCLKX_OFF ); //MCASP1_AXR1   MODE3

/* Pad functional signal mux select. */
#define MODE0                                   0b000
#define MODE1                                   0b001
#define MODE2                                   0b010
#define MODE3                                   0b011
#define MODE4                                   0b100
#define MODE5                                   0b101
#define MODE6                                   0b110
#define MODE7                                   0b111
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

/* For the mcaspx_aclkx, mcaspx_ahclkx and mcaspx_aclkr signals to work properly,
 * the INPUTENABLE bit of the appropriate CTRL_CORE_PAD_x registers should be set to 0x1
 * because of retiming purposes. */
void config_mcasp0_pads_BBB(void)
{
    * CONF_LCD_DATA8_PTR        = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL) | CONF_MODULE_PIN_RX_ACTIVE_VAL(RX_ACTIVE_EN); //MCASP0_ACLKX  MODE3
#if (CDC_IS_MASTER)
    * CONF_LCD_DATA9_PTR        = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL) | CONF_MODULE_PIN_RX_ACTIVE_VAL(RX_ACTIVE_EN); //MCASP0_FSX    MODE3 IN
#else
    * CONF_LCD_DATA9_PTR        = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);                                               //MCASP0_FSX    MODE3 OUT
#endif
    * CONF_LCD_DATA10_PTR       = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);                                               //MCASP0_AXR0   MODE3 TX
    * CONF_LCD_DATA14_PTR       = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL) | CONF_MODULE_PIN_RX_ACTIVE_VAL(RX_ACTIVE_EN); //MCASP0_AXR1   MODE3 RX
}

void config_mcasp1_pads_BBB(void)
{
    * CONF_MCASP0_ACLKR_PTR     = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL) | CONF_MODULE_PIN_RX_ACTIVE_VAL(RX_ACTIVE_EN); //MCASP1_ACLKX  MODE3
#if (CDC_IS_MASTER)
    * CONF_MCASP0_FSR_PTR       = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL) | CONF_MODULE_PIN_RX_ACTIVE_VAL(RX_ACTIVE_EN); //MCASP1_FSX    MODE3 IN
#else
    * CONF_MCASP0_FSR_PTR       = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);                                               //MCASP1_FSX    MODE3 OUT
#endif
    * CONF_MCASP0_AXR1_PTR      = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL);                                               //MCASP1_AXR0   MODE3 TX
    * CONF_MCASP0_AHCLKX_PTR    = CONF_MODULE_PIN_MODE_VAL(MODE3) | CONF_MODULE_PIN_MODE_PUDEN_VAL(PULL_EN) | CONF_MODULE_PIN_MODE_PUTYPESEL_VAL(PULLUP_SEL) | CONF_MODULE_PIN_RX_ACTIVE_VAL(RX_ACTIVE_EN); //MCASP1_AXR1   MODE3 RX
}
#endif /* RUN_ON_ARM */
