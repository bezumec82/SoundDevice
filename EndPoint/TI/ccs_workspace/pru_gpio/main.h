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

#define LIGHT_SHOW              FALSE

#endif
