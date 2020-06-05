#ifndef __CM_REGS_H__
#define __CM_REGS_H__

#include "main.h"

#define MODULEMODE_ENABLE           0x2
#define MODULEMODE_DISABLE          0x0

#define CM_L4PER2_MCASP8_CLKCTRL_ADDR           0x4A009890
#define CM_IPU_MCASP1_CLKCTRL_ADDR              0x4A005550
struct MCASP_CLKCTRL {
    uint32_t MODULEMODE             : 2;
    uint32_t RESERVED_2             : 14;
    uint32_t IDLEST                 : 2;
    uint32_t RESERVED_21            : 4;
    uint32_t CLKSEL_AUX_CLK         : 2;
    uint32_t CLKSEL_AHCLKX          : 4;
    uint32_t CLKSEL_AHCLKR          : 4;
};

#define SW_WKUP                     0x2 /* Start a software forced wake-up transition on the domain. */
#define CM_IPU_CLKSTCTRL_ADDR       0x4A005540

/* Select the PER_ABE_X1_GFCLK_CLK for McASP subsystems. */
#define CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX_ADDR    0x4AE06138
struct CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX {
    uint32_t CLKSEL                 : 3;
};

/* Control the source of the SYS clock for DPLL_ABE */
#define CM_CLKSEL_ABE_PLL_SYS_ADDR  0x4AE06118
struct CM_CLKSEL_ABE_PLL_SYS {
    uint32_t CLKSEL                 : 1;
};

#define CM_CLKSEL_DPLL_ABE_ADDR     0x4A0051EC
struct CM_CLKSEL_DPLL_ABE {
    uint32_t DPLL_DIV               : 7;
    uint32_t RESERVED_7             : 1;
    uint32_t DPLL_MULT              : 11;
    uint32_t RESERVED_19            : 3;
    uint32_t DCC_EN                 : 1;
    uint32_t DPLL_BYP_CLKSEL        : 1;
    uint32_t RESERVED_24            : 8;
};

#define CM_CLKMODE_DPLL_ABE_ADDR    0x4A0051E0
#define DPLL_EN_LOCK_MODE           0x7
#define DPLL_EN_FR_BYP_MODE         0x6
struct CM_CLKMODE_DPLL_ABE {
    uint32_t DPLL_EN                : 3;
    uint32_t RESERVED_3             : 5;
    uint32_t DPLL_DRIFTGUARD_EN     : 1;
    uint32_t RESERVED_9             : 1;
    uint32_t DPLL_LPMODE_EN         : 1;
    uint32_t DPLL_REGM4XEN          : 1;
    uint32_t DPLL_SSC_EN            : 1;
    uint32_t DPLL_SSC_ACK           : 1;
    uint32_t DPLL_SSC_DOWNSPREAD    : 1;
    uint32_t DPLL_SSC_TYPE          : 1;
    uint32_t RESERVED_16            : 16;
};

#define CM_DIV_M2_DPLL_ABE_ADDR     0x4A0051F0
struct CM_DIV_M2_DPLL_ABE {
    uint32_t DIVHS                  : 5;
    uint32_t RESERVED_5             : 4;
    uint32_t CLKST                  : 1;
    uint32_t RESERVED_10            : 1;
    uint32_t CLKX2ST                : 1;
    uint32_t RESERVED_12            : 20;
};

#define CM_DIV_M3_DPLL_ABE_ADDR     0x4A0051F4
struct CM_DIV_M3_DPLL_ABE {
    uint32_t DIVHS                  : 5;
    uint32_t RESERVED_5             : 4;
    uint32_t CLKST                  : 1;
    uint32_t RESERVED_10            : 22;
};

#define CM_CLKSEL_CLKOUTMUX2_ADDR               0x4AE06160
#define CM_CLKSEL_PER_ABE_X1_CLK_CLKOUTMUX      0x2
struct CM_CLKSEL_CLKOUTMUX2 {
    uint32_t CLKSEL                 : 5;
    uint32_t RESERVED_5             : 27;
};

#define CM_COREAON_CLKOUTMUX2_CLKCTRL_ADDR      0x4A0086B0
struct CM_COREAON_CLKOUTMUX2_CLKCTRL {
    uint32_t RESERVED_0                 : 8;
    uint32_t OPTFCLKEN_CLKOUTMUX2_CLK   : 1;
};

#endif
