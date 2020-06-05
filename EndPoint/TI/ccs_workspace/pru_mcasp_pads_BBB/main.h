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

#define PRU0                    TRUE
#define SETUP_DPLL              TRUE

#include "rpmsg.h"

struct CONF_MODULE_PIN {
    uint32_t MODE : 3;
    uint32_t PUDEN : 1;
    uint32_t PUTYPESEL : 1;
    uint32_t RXACTIVE : 1;
    uint32_t SLEWCTRL : 1;
};

#endif /* __MAIN_H__ */
