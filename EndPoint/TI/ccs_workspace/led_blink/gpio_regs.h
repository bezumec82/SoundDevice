#ifndef __GPIO_REGS_H__
#define __GPIO_REGS_H__

#define GPIO0_REGS                              0x44E07000
#define GPIO1_REGS                              0x4804C000
#define GPIO2_REGS                              0x481AC000
#define GPIO3_REGS                              0x481AE000

#define GPIO_SYSCONFIG_OFF                      0x10
#define GPIO_SYSSTATUS_OFF                      0x114
#define GPIO_CTRL_OFF                           0x130
#define GPIO_OE_OFF                             0x134
#define GPIO_DATAOUT_OFF                        0x13C
#define GPIO_CLEARDATAOUT_OFF                   0x190
#define GPIO_SETDATAOUT_OFF                     0x194

struct GPIO_SYSCONFIG {
    uint32_t AUTOIDLE   : 1;
    uint32_t SOFTRESET  : 1;
    uint32_t ENAWAKEUP  : 1;
    uint32_t IDLEMODE   : 2;
};

struct GPIO_CTRL {
    uint32_t DISABLEMODULE : 1;
    uint32_t GATINGRATIO   : 2;
};

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

struct GPIO_DATAOUT {
    uint32_t GPIO_0 : 1;
    uint32_t GPIO_1 : 1;
    uint32_t GPIO_2 : 1;
    uint32_t GPIO_3 : 1;
    uint32_t GPIO_4 : 1;
    uint32_t GPIO_5 : 1;
    uint32_t GPIO_6 : 1;
    uint32_t GPIO_7 : 1;
    uint32_t GPIO_8 : 1;
    uint32_t GPIO_9 : 1;

    uint32_t GPIO_10 : 1;
    uint32_t GPIO_11 : 1;
    uint32_t GPIO_12 : 1;
    uint32_t GPIO_13 : 1;
    uint32_t GPIO_14 : 1;
    uint32_t GPIO_15 : 1;
    uint32_t GPIO_16 : 1;
    uint32_t GPIO_17 : 1;
    uint32_t GPIO_18 : 1;
    uint32_t GPIO_19 : 1;

    uint32_t GPIO_20 : 1;
    uint32_t GPIO_21 : 1;
    uint32_t GPIO_22 : 1;
    uint32_t GPIO_23 : 1;
    uint32_t GPIO_24 : 1;
    uint32_t GPIO_25 : 1;
    uint32_t GPIO_26 : 1;
    uint32_t GPIO_27 : 1;
    uint32_t GPIO_28 : 1;
    uint32_t GPIO_29 : 1;

    uint32_t GPIO_30 : 1;
    uint32_t GPIO_31 : 1;
};

#endif /* __GPIO_REGS_H__ */
