
#include <stdint.h>
#include <pru_cfg.h>

volatile register uint32_t __R30;

/*
 * main.c
 */
int main(void)
{
    CT_CFG.GPCFG0 = 0;
    while (1) {
        __R30 = 0xffffffff;
        __delay_cycles(100000000);
        __R30 = 0x0;
        __delay_cycles(100000000);
    }
}
