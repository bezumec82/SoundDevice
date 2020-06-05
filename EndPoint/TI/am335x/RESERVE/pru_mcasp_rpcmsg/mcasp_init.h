#ifndef __MCASP_INIT_H__
#define __MCASP_INIT_H__

#include "main.h"

struct rpmsg_data;

#define INIT_TIMEOUT_CNT                200 * 1000 * 1000 //1 sec
#define WAIT_REG_SET(reg_bit)               \
        {                                   \
            uint32_t timeout_counter = 0;   \
            while (!reg_bit)                \
            {                               \
                service_buffs();            \
                timeout_counter++;          \
                if (timeout_counter>INIT_TIMEOUT_CNT)   \
                {                           \
                    goto start;             \
                }                           \
            };                              \
        };

#define WAIT_REG_RESET(reg_bit)             \
        {                                   \
            uint32_t timeout_counter = 0;   \
            while (reg_bit)                 \
            {                               \
                service_buffs();            \
                timeout_counter++;          \
                if (timeout_counter>INIT_TIMEOUT_CNT)   \
                {                           \
                    goto start;             \
                }                           \
            };                              \
        };
/*---------*/
/* McASP 1 */
/*---------*/
extern struct MCASP_STAT *         MCASP_TXSTAT_bits_ptr;
extern struct MCASP_STAT *         MCASP_RXSTAT_bits_ptr;
extern struct MCASP_TDMSLOT *      MCASP_TXTDMSLOT_bits_ptr;
extern struct MCASP_TDMSLOT *      MCASP_RXTDMSLOT_bits_ptr;
extern uint32_t *                  MCASP_TXBUF_ptr;
extern uint32_t *                  MCASP_RXBUF_ptr;
extern uint32_t *                  MCASP_GBLCTL_ptr;

void init_mcasp ();

#endif /* __MCASP_INIT_H__ */
