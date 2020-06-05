#include "main.h"

volatile struct CM_COREAON_CLKOUTMUX2_CLKCTRL * CM_COREAON_CLKOUTMUX2_CLKCTRL_ptr          = (struct CM_COREAON_CLKOUTMUX2_CLKCTRL *)  CM_COREAON_CLKOUTMUX2_CLKCTRL_ADDR;
volatile struct CM_CLKSEL_CLKOUTMUX2 *          CM_CLKSEL_CLKOUTMUX2_ptr                   = (struct CM_CLKSEL_CLKOUTMUX2 *)           CM_CLKSEL_CLKOUTMUX2_ADDR;
volatile struct CM_L4PER2_MCASP8_CLKCTRL *      CM_L4PER2_MCASP8_CLKCTRL_ptr               = (struct CM_L4PER2_MCASP8_CLKCTRL *)       CM_L4PER2_MCASP8_CLKCTRL_ADDR;

volatile struct CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX *
                                                CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX_ptr   = (struct CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX *)CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX_ADDR;

#if (SETUP_DPLL_ABE)
volatile struct CM_CLKMODE_DPLL_ABE *           CM_CLKMODE_DPLL_ABE_ptr                    = (struct CM_CLKMODE_DPLL_ABE *)            CM_CLKMODE_DPLL_ABE_ADDR;
volatile struct CM_CLKSEL_DPLL_ABE *            CM_CLKSEL_DPLL_ABE_ptr                     = (struct CM_CLKSEL_DPLL_ABE *)             CM_CLKSEL_DPLL_ABE_ADDR;
volatile struct CM_DIV_M2_DPLL_ABE *            CM_DIV_M2_DPLL_ABE_ptr                     = (struct CM_DIV_M2_DPLL_ABE *)             CM_DIV_M2_DPLL_ABE_ADDR;
volatile struct CM_DIV_M3_DPLL_ABE *            CM_DIV_M3_DPLL_ABE_ptr                     = (struct CM_DIV_M3_DPLL_ABE *)             CM_DIV_M3_DPLL_ABE_ADDR;
volatile struct CM_CLKSEL_ABE_PLL_SYS *         CM_CLKSEL_ABE_PLL_SYS_ptr                  = (struct CM_CLKSEL_ABE_PLL_SYS *)          CM_CLKSEL_ABE_PLL_SYS_ADDR;
#endif


void clk_cfg (void)
{
    CM_CLKSEL_CLKOUTMUX2_ptr->CLKSEL = CM_CLKSEL_PER_ABE_X1_CLK_CLKOUTMUX;

    /* Disable module */
    CM_L4PER2_MCASP8_CLKCTRL_ptr->MODULEMODE = MODULEMODE_DISABLE;
    __delay_cycles(100000000);

#if (MCASP8_TX||MCASP8_RX)
    while (CM_L4PER2_MCASP8_CLKCTRL_ptr->MODULEMODE != MODULEMODE_ENABLE)
        CM_L4PER2_MCASP8_CLKCTRL_ptr->MODULEMODE = MODULEMODE_ENABLE; /* Module is explicitly enabled. */
    CM_L4PER2_MCASP8_CLKCTRL_ptr->CLKSEL_AHCLKX = 0x0;  /* ABE_SYS_CLK */
    CM_L4PER2_MCASP8_CLKCTRL_ptr->CLKSEL_AUX_CLK = 0x0; /* PER_ABE_X1_GFCLK */
#endif
    CM_CLKSEL_ABE_PLL_SYS_ptr->CLKSEL = 0x0; /* !!! */




#if (SETUP_DPLL_ABE)
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
}
