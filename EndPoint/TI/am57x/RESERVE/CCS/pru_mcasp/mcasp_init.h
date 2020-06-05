#ifndef __MCASP_INIT_H__
#define __MCASP_INIT_H__

#include "main.h"

void service_buffs (void);

#define INIT_TIMEOUT_CNT                200 * 1000 * 1000 //1 sec
#define WAIT_REG_SET(reg_bit)               \
        if (1)                              \
        {                                   \
            uint32_t timeout_counter = 0;   \
            while (!reg_bit)                \
            {                               \
                service_buffs();            \
                timeout_counter++;          \
                if (timeout_counter>INIT_TIMEOUT_CNT)   \
                {                           \
                    set_status_0_red();     \
                    goto start;             \
                }                           \
            };                              \
        };

#define WAIT_REG_RESET(reg_bit)             \
        if (1)                              \
        {                                   \
            uint32_t timeout_counter = 0;   \
            while (reg_bit)                 \
            {                               \
                service_buffs();            \
                timeout_counter++;          \
                if (timeout_counter>INIT_TIMEOUT_CNT)   \
                {                           \
                    set_status_0_red();     \
                    goto start;             \
                }                           \
            };                              \
        };

/*---------*/
/* McASP 8 */
/*---------*/
extern struct MCASP_STAT *         MCASP8_TXSTAT_bits_ptr;
extern uint32_t *                  MCASP8_TXSTAT_ptr;
extern struct MCASP_STAT *         MCASP8_RXSTAT_bits_ptr;
extern uint32_t *                  MCASP8_RXSTAT_ptr;

extern struct MCASP_TDMSLOT *      MCASP8_TXTDMSLOT_bits_ptr;
extern struct MCASP_TDMSLOT *      MCASP8_RXTDMSLOT_bits_ptr;

extern uint32_t *                  MCASP8_TXBUF_ptr;
extern uint32_t *                  MCASP8_RXBUF_ptr;

void init_mcasp8 (void);
/*---------*/
/* McASP 1 */
/*---------*/
extern struct MCASP_STAT *         MCASP1_TXSTAT_bits_ptr;
extern struct MCASP_STAT *         MCASP1_RXSTAT_bits_ptr;
extern struct MCASP_TDMSLOT *      MCASP1_TXTDMSLOT_bits_ptr;
extern struct MCASP_TDMSLOT *      MCASP1_RXTDMSLOT_bits_ptr;
extern uint32_t *                  MCASP1_TXBUF_ptr;
extern uint32_t *                  MCASP1_RXBUF_ptr;

void init_mcasp1 (void);

void init_mcasp(void);

#endif
