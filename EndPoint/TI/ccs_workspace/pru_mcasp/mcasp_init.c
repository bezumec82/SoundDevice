#include "mcasp_init.h"

/* This function should take into account all McASPs present in the programm */
void service_buffs (void)
{
    uint32_t dump = 0;
#if (MCASP8_RX)
    if (MCASP8_RXSTAT_bits_ptr->DATA) //service all RX buffs
    {
        dump = * MCASP8_RXBUF_ptr;
    }
#endif
#if (MCASP8_TX)
    if (MCASP8_TXSTAT_bits_ptr->DATA) //service all TX bufs
    {
        * MCASP8_TXBUF_ptr =dump;
    }
#endif
#if (MCASP1_RX)
    if (MCASP1_RXSTAT_bits_ptr->DATA) //service all RX buffs
    {
        dump = * MCASP1_RXBUF_ptr;
    }
#endif
#if (MCASP1_TX)
    if (MCASP1_TXSTAT_bits_ptr->DATA) //service all TX bufs
    {
        * MCASP1_TXBUF_ptr =dump;
    }
#endif
}
/*--- End of the function ----------------------------------------------------*/


void init_mcasp (void)
{
#if (MCASP1_TX||MCASP1_RX)
    init_mcasp1();
#endif

#if (MCASP8_TX||MCASP8_RX)
    init_mcasp8();
#endif
}
/*--- End of the function ----------------------------------------------------*/

void restart_mcasp8_TX (void)
{
    /*
     * Cause RX is synchronous to TX, it is impossible
     * to restart TX part separately from RX part.
     * To restart TX part of the module, whole module
     * should be placed in reset state and reconfigured.
     */
    init_mcasp();
}
/*--- End of the function ----------------------------------------------------*/
