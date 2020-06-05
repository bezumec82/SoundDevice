#include "main.h"

#if (IDK_AM5728)
volatile struct CM_COREAON_CLKOUTMUX2_CLKCTRL * CM_COREAON_CLKOUTMUX2_CLKCTRL_ptr          = (struct CM_COREAON_CLKOUTMUX2_CLKCTRL *)  CM_COREAON_CLKOUTMUX2_CLKCTRL_ADDR;
volatile struct CM_CLKSEL_CLKOUTMUX2 *          CM_CLKSEL_CLKOUTMUX2_ptr                   = (struct CM_CLKSEL_CLKOUTMUX2 *)           CM_CLKSEL_CLKOUTMUX2_ADDR;
volatile struct MCASP_CLKCTRL *                 CM_L4PER2_MCASP8_CLKCTRL_ptr               = (struct MCASP_CLKCTRL *)                  CM_L4PER2_MCASP8_CLKCTRL_ADDR;
volatile struct MCASP_CLKCTRL *                 CM_IPU_MCASP1_CLKCTRL_bits_ptr             = (struct MCASP_CLKCTRL *)                  CM_IPU_MCASP1_CLKCTRL_ADDR;
volatile        uint32_t *                      CM_IPU_MCASP1_CLKCTRL_ptr                  = (uint32_t *)                              CM_IPU_MCASP1_CLKCTRL_ADDR;
volatile struct CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX *
                                                CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX_ptr   = (struct CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX *)CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX_ADDR;
volatile        uint32_t *                      CM_IPU_CLKSTCTRL_ADDR_ptr                  = (uint32_t *)                               CM_IPU_CLKSTCTRL_ADDR;

#if (SETUP_DPLL_ABE)
    volatile struct CM_CLKMODE_DPLL_ABE *   CM_CLKMODE_DPLL_ABE_ptr   = (struct CM_CLKMODE_DPLL_ABE *)   CM_CLKMODE_DPLL_ABE_ADDR;
    volatile struct CM_CLKSEL_DPLL_ABE *    CM_CLKSEL_DPLL_ABE_ptr    = (struct CM_CLKSEL_DPLL_ABE *)    CM_CLKSEL_DPLL_ABE_ADDR;
    volatile struct CM_DIV_M2_DPLL_ABE *    CM_DIV_M2_DPLL_ABE_ptr    = (struct CM_DIV_M2_DPLL_ABE *)    CM_DIV_M2_DPLL_ABE_ADDR;
    volatile struct CM_DIV_M3_DPLL_ABE *    CM_DIV_M3_DPLL_ABE_ptr    = (struct CM_DIV_M3_DPLL_ABE *)    CM_DIV_M3_DPLL_ABE_ADDR;
    //volatile struct CM_CLKSEL_ABE_PLL_SYS * CM_CLKSEL_ABE_PLL_SYS_ptr = (struct CM_CLKSEL_ABE_PLL_SYS *) CM_CLKSEL_ABE_PLL_SYS_ADDR;
    volatile        uint32_t *              CM_CLKSEL_ABE_PLL_SYS_ptr = (uint32_t *)CM_CLKSEL_ABE_PLL_SYS_ADDR;
#endif

void clk_cfg (void)
{
#if (SETUP_DPLL_ABE)
        * CM_CLKSEL_ABE_PLL_SYS_ptr = 0x0; // !!! Set SYS_CLK1 as ABE source !!!
        while (CM_CLKMODE_DPLL_ABE_ptr->DPLL_EN != DPLL_EN_FR_BYP_MODE)
            CM_CLKMODE_DPLL_ABE_ptr->DPLL_EN = DPLL_EN_FR_BYP_MODE;
        // step 2: modify Synthesized Clock Parameters - DPLL MULT & DIV
        CM_CLKSEL_DPLL_ABE_ptr->DPLL_MULT = 100; /* PER_ABE_X1_GFCLK -> MCASP8_AUX_GFCLK = 100 MHz */
        CM_CLKSEL_DPLL_ABE_ptr->DPLL_DIV = 19; /* Divide by 20 */
        // step 3: Configure output clocks parameters - M2 = 1  M3 = 1
        CM_DIV_M2_DPLL_ABE_ptr->DIVHS = 0x1;
        CM_DIV_M3_DPLL_ABE_ptr->DIVHS = 0x1;
        // step 4: Confirm that the PLL has locked
        while (CM_CLKMODE_DPLL_ABE_ptr->DPLL_EN != DPLL_EN_LOCK_MODE)
            CM_CLKMODE_DPLL_ABE_ptr->DPLL_EN = DPLL_EN_LOCK_MODE;
#endif

#if 	(MCASP_CFG_ADDR == MCASP8_CFG_ADDR)
    /* Disable module */
//    CM_L4PER2_MCASP8_CLKCTRL_ptr->MODULEMODE = MODULEMODE_DISABLE;
    * CM_IPU_MCASP1_CLKCTRL_ptr = 0x0;
    __delay_cycles(100000000);
    while (CM_L4PER2_MCASP8_CLKCTRL_ptr->MODULEMODE != MODULEMODE_ENABLE)
        CM_L4PER2_MCASP8_CLKCTRL_ptr->MODULEMODE = MODULEMODE_ENABLE; /* Module is explicitly enabled. */
//    CM_L4PER2_MCASP8_CLKCTRL_ptr->CLKSEL_AUX_CLK  = 0x0;
//    CM_L4PER2_MCASP8_CLKCTRL_ptr->CLKSEL_AHCLKX   = 0x0;
//    CM_L4PER2_MCASP8_CLKCTRL_ptr->CLKSEL_AHCLKR   = 0x0;
#elif   (MCASP_CFG_ADDR == MCASP1_CFG_ADDR)
    * CM_IPU_CLKSTCTRL_ADDR_ptr = SW_WKUP;
//    CM_IPU_MCASP1_CLKCTRL_bits_ptr->MODULEMODE = MODULEMODE_DISABLE;
    * CM_IPU_MCASP1_CLKCTRL_ptr = 0x0;
    __delay_cycles(100000000);
    while (CM_IPU_MCASP1_CLKCTRL_bits_ptr->MODULEMODE != MODULEMODE_ENABLE)
        CM_IPU_MCASP1_CLKCTRL_bits_ptr->MODULEMODE = MODULEMODE_ENABLE; /* Module is explicitly enabled. */
//    CM_IPU_MCASP1_CLKCTRL_bits_ptr->CLKSEL_AUX_CLK    = 0x0;
//    CM_IPU_MCASP1_CLKCTRL_bits_ptr->CLKSEL_AHCLKX     = 0x0;
//    CM_IPU_MCASP1_CLKCTRL_bits_ptr->CLKSEL_AHCLKR     = 0x0;
#endif
    CM_CLKSEL_CLKOUTMUX2_ptr->CLKSEL = CM_CLKSEL_PER_ABE_X1_CLK_CLKOUTMUX;
}
#endif /* IDK_AM5728 */

#if (BBB_AM335x)
#define CM_WKUP_CONTROL_CLKCTRL_ADDR            0x44E00404
volatile uint32_t * CM_WKUP_CONTROL_CLKCTRL_PTR          = (uint32_t *)( CM_WKUP_CONTROL_CLKCTRL_ADDR );
/* -------------------------------------------------------------------------- */

#define CONTROL_MODULE_ADDR                     0x44E10000
#define CONTROL_SYSCONFIG_OFF                   0x10
#define CONTROL_DEV_ID_OFF                      0x600
volatile uint32_t * CONTROL_SYSCONFIG_PTR                = (uint32_t *)(CONTROL_MODULE_ADDR + CONTROL_SYSCONFIG_OFF);
#define FREEEMU_DIS                             (uint32_t)(0b1 << 1)
#define IDLEMODE_NO_IDLE                        (uint32_t)(0b01 << 2)
#define STANDBY_NO_STANDBY                      (uint32_t)(0b01 << 4)
/* -------------------------------------------------------------------------- */


#define CM_PER_MCASP0_CLKCTRL_ADDR              0x44E00034
#define CM_PER_MCASP1_CLKCTRL_ADDR              0x44E00068
#if     (MCASP0)
volatile uint32_t * CM_PER_MCASP_CLKCTRL_PTR             = (uint32_t *)( CM_PER_MCASP0_CLKCTRL_ADDR );
#elif   (MCASP1)
uint32_t * CM_PER_MCASP_CLKCTRL_PTR             = (uint32_t *)( CM_PER_MCASP1_CLKCTRL_ADDR );
#endif
/* -------------------------------------------------------------------------- */

#if (SETUP_DPLL)
#define CM_CLKSEL_DPLL_CORE_ADDR        0x44E00468
#define DPLL_MULT(x)                    (uint32_t)((x & 0b11111111111) << 8)
#define DPLL_DIV(x)                     (uint32_t)(x & 0b1111111)
uint32_t * CM_CLKSEL_DPLL_CORE_PTR      = (uint32_t *)( CM_CLKSEL_DPLL_CORE_ADDR );

#define CM_CLKMODE_DPLL_CORE_ADDR       0x44E00490
#define DPLL_EN_MASK                    0b111
#define DPLL_EN(x)                      (x & DPLL_EN_MASK)
#define MN_BYPASS                       0b100
uint32_t * CM_CLKMODE_DPLL_CORE_PTR     = (uint32_t *)( CM_CLKMODE_DPLL_CORE_ADDR );

#define CM_DIV_M4_DPLL_CORE_ADDR        0x44E00480
#define CM_DIV_M5_DPLL_CORE_ADDR        0x44E00484
uint32_t * CM_DIV_M4_DPLL_CORE_PTR      = (uint32_t *)( CM_DIV_M4_DPLL_CORE_ADDR );
uint32_t * CM_DIV_M5_DPLL_CORE_PTR      = (uint32_t *)( CM_DIV_M5_DPLL_CORE_ADDR );
#define HSDIVIDER_CLKOUT_DIV(x)        (x & 0b11111)
#define ST_HSDIVIDER_CLKOUT            1<<9
#endif
/* -------------------------------------------------------------------------- */

void clk_cfg (void)
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
    //* CONTROL_SYSCONFIG_PTR = FREEEMU_DIS | IDLEMODE_NO_IDLE | STANDBY_NO_STANDBY;
#if (0)
    uint32_t CM_WKUP_CONTROL_CLKCTRL_val = * CM_WKUP_CONTROL_CLKCTRL_PTR;
#endif
    * CM_PER_MCASP_CLKCTRL_PTR      = MODULEMODE_ENABLE;
}
#endif /* BBB_AM335x */


