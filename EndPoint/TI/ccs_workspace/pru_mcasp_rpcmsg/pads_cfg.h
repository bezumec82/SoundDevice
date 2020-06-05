#ifndef __PADS_CFG_H__
#define __PADS_CFG_H__

#include "main.h"

struct CTRL_CORE_PAD {
    uint32_t MUXMODE        : 4;
    uint32_t DELAYMODE      : 4;
    uint32_t MODESELECT     : 1;
    uint32_t RESERVED_9     : 7;
    uint32_t PULLUDENABLE   : 1;
    uint32_t PULLTYPESELECT : 1;
    uint32_t INPUTENABLE    : 1;
    uint32_t SLEWCONTROL    : 1;
    uint32_t RESERVED_20    : 4;
    uint32_t WAKEUPENABLE   : 1;
    uint32_t WAKEUPEVENT    : 1;
    uint32_t RESERVED_26    : 6;
};


#if (0)
void config_mcasp0_pads_BBB(void);
void config_mcasp1_pads_BBB(void);
#endif
/* -------------------------------------------------------------------------- */

void config_mcasp8_pads_idk_am5728(void);
void config_mcasp1_pads_idk_am5728(void);
/* -------------------------------------------------------------------------- */

void setup_gpio_pads(void);

void set_status_0_red (void);
void set_status_1_red (void);

void set_industial_2_red (void);
void set_industial_3_red (void);

void set_status_0_green (void);
void set_status_1_green (void);

void set_industial_2_green (void);
void set_industial_3_green (void);


void set_status_0_blue (void);
void set_status_1_blue (void);

void set_industial_2_blue (void);
void set_industial_3_blue (void);
/* -------------------------------------------------------------------------- */


#endif /* __PADS_CFG_H__ */
