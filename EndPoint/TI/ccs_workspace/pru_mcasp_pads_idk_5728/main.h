#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>

#include "gpio_regs.h"

#define TRUE                    1
#define FALSE                   0

#define PRU0_MCASP1             TRUE
#include "rpmsg.h"

struct GPIO_OE {
    uint32_t OUTPUTEN_0 : 1;
    uint32_t OUTPUTEN_1 : 1;
    uint32_t OUTPUTEN_2 : 1;
    uint32_t OUTPUTEN_3 : 1;
    uint32_t OUTPUTEN_4 : 1;
    uint32_t OUTPUTEN_5 : 1;
    uint32_t OUTPUTEN_6 : 1;
    uint32_t OUTPUTEN_7 : 1;
    uint32_t OUTPUTEN_8 : 1;
    uint32_t OUTPUTEN_9 : 1;

    uint32_t OUTPUTEN_10 : 1;
    uint32_t OUTPUTEN_11 : 1;
    uint32_t OUTPUTEN_12 : 1;
    uint32_t OUTPUTEN_13 : 1;
    uint32_t OUTPUTEN_14 : 1;
    uint32_t OUTPUTEN_15 : 1;
    uint32_t OUTPUTEN_16 : 1;
    uint32_t OUTPUTEN_17 : 1;
    uint32_t OUTPUTEN_18 : 1;
    uint32_t OUTPUTEN_19 : 1;

    uint32_t OUTPUTEN_20 : 1;
    uint32_t OUTPUTEN_21 : 1;
    uint32_t OUTPUTEN_22 : 1;
    uint32_t OUTPUTEN_23 : 1;
    uint32_t OUTPUTEN_24 : 1;
    uint32_t OUTPUTEN_25 : 1;
    uint32_t OUTPUTEN_26 : 1;
    uint32_t OUTPUTEN_27 : 1;
    uint32_t OUTPUTEN_28 : 1;
    uint32_t OUTPUTEN_29 : 1;

    uint32_t OUTPUTEN_30 : 1;
    uint32_t OUTPUTEN_31 : 1;
};


#endif
