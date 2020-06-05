#ifndef __GPIO_REGS_H__
#define __GPIO_REGS_H__

/*
 * GPIO7        GPIO8       GPIO2       GPIO3       GPIO4       GPIO5       GPIO6       GPIO1
 * 0x48051010   0x48053010  0x48055010  0x48057010  0x48059010  0x4805B010  0x4805D010  0x4AE10010
 */
#define GPIO1_SYSCONFIG_ADDR    0x4AE10010

#define GPIO2_SYSCONFIG_ADDR    0x48055010
#define GPIO3_SYSCONFIG_ADDR    0x48057010
#define GPIO4_SYSCONFIG_ADDR    0x48059010
#define GPIO5_SYSCONFIG_ADDR    0x4805B010
#define GPIO6_SYSCONFIG_ADDR    0x4805D010

#define GPIO7_SYSCONFIG_ADDR    0x48051010
#define GPIO8_SYSCONFIG_ADDR    0x48053010
struct GPIO_SYSCONFIG {
    uint32_t AUTOIDLE   : 1;
    uint32_t SOFTRESET  : 1;
    uint32_t ENAWAKEUP  : 1;
    uint32_t IDLEMODE   : 1;
};
/*
 * GPIO7        GPIO8       GPIO2       GPIO3       GPIO4       GPIO5       GPIO6       GPIO1
 * 0x48051134   0x48053134  0x48055134  0x48057134  0x48059134  0x4805B134  0x4805D134  0x4AE10134
 */
#define GPIO1_OE_ADDR           0x4AE10134

#define GPIO2_OE_ADDR           0x48055134
#define GPIO3_OE_ADDR           0x48057134
#define GPIO4_OE_ADDR           0x48059134
#define GPIO5_OE_ADDR           0x4805B134
#define GPIO6_OE_ADDR           0x4805D134

#define GPIO7_OE_ADDR           0x48051134
#define GPIO8_OE_ADDR           0x48053134
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

/*
 *  GPIO7           GPIO8           GPIO2           GPIO3           GPIO4           GPIO5           GPIO6           GPIO1
 *  0x4805113C      0x4805313C      0x4805513C      0x4805713C      0x4805913C      0x4805B13C      0x4805D13C      0x4AE1013C
 */
#define GPIO1_DATAOUT_ADDR      0x4AE1013C

#define GPIO2_DATAOUT_ADDR      0x4805513C
#define GPIO3_DATAOUT_ADDR      0x4805713C
#define GPIO4_DATAOUT_ADDR      0x4805913C
#define GPIO5_DATAOUT_ADDR      0x4805B13C
#define GPIO6_DATAOUT_ADDR      0x4805D13C

#define GPIO7_DATAOUT_ADDR      0x4805113C
#define GPIO8_DATAOUT_ADDR      0x4805313C

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

#endif
