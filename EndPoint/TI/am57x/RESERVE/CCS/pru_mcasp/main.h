#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>
#include <pru_cfg.h>

#include "mcasp_regs.h"
#include "cm_regs.h"
#include "gpio_regs.h"
#include "pads_cfg.h"
#include "mcasp_init.h"

#define TRUE                    1
#define FALSE                   0

#define SETUP_DPLL_ABE          TRUE
#define DEBUG_LEDS              TRUE
#define TX_DEBUG                TRUE
#define BUF_DEBUG               TRUE //TX output will show numbers
#define TX_RUNTIME_DEBUG        FALSE //The same but numbers will fill dynamically

#define MCASP1_TX               TRUE
#define MCASP1_RX               FALSE

#define MCASP8_TX               TRUE
#define MCASP8_RX               TRUE

#define DEBUG_MCASP_ERRS        FALSE

#define BUFS_ABSTRACTION        FALSE
#define MCASP_ABSTRACTION       FALSE

/* Ideally in final system this address
 * (address where PRU should allocate its buffs)
 * should be sent through mailbox from Linux kernel.
 */
#define DDR_START_ADDR          0x80000000 + 0x01000000

/*
 * Packages at upper level arrives in 20ms size.
 * We should have possibility place 2 packages ahead.
 * 1 buf = 64 words == 4 ms (16kHz = 16 words per ms)
 * 2 * 20 + 4 = 44 ms == 11 bufs (buf in work and two 20ms frames ahead)
 */
#define BUF_SIZE_WORDS          64
#define BUF_SIZE_BYTES          BUF_SIZE_WORDS * 4

#define NUM_TX8_SLOTS           2
#define NUM_RX8_SLOTS           2
#define NUM_TX8_BUFS            16
#define NUM_RX8_BUFS            16

#define NUM_TX1_SLOTS           2
#define NUM_RX1_SLOTS           2

#define NUM_TX1_BUFS            16
#define NUM_RX1_BUFS            16

/*
 * TX8 sends data to RX1, RX1 and TX1 have the same buffers,
 * TX1 output data received by RX1
 */
#define ECHO_TEST_TX8_to_RX1            FALSE
/* McASP1 sends data to McASP8, McASP8 runs data to output */
#define ECHO_TEST_TX1_to_RX8            TRUE

/* Sequential buffer positions */
#define MCASP8_TX_BUFS_START_ADDR       (uint32_t)  DDR_START_ADDR

#if (ECHO_TEST_TX1_to_RX8)
#define MCASP8_RX_BUFS_START_ADDR       (uint32_t)  MCASP8_TX_BUFS_START_ADDR
#else
#define MCASP8_RX_BUFS_START_ADDR       (uint32_t)  ( DDR_START_ADDR \
                                                    + (NUM_TX8_SLOTS * NUM_TX8_BUFS * BUF_SIZE_BYTES) )
#endif
#define MCASP1_TX_BUFS_START_ADDR       (uint32_t)  ( DDR_START_ADDR \
                                                    + (NUM_TX8_SLOTS * NUM_TX8_BUFS * BUF_SIZE_BYTES) \
                                                    + (NUM_RX8_SLOTS * NUM_RX8_BUFS * BUF_SIZE_BYTES) )
#if (ECHO_TEST_TX8_to_RX1)
#define MCASP1_RX_BUFS_START_ADDR       (uint32_t)  MCASP1_TX_BUFS_START_ADDR

#else
#define MCASP1_RX_BUFS_START_ADDR       (uint32_t)  ( DDR_START_ADDR \
                                                    + (NUM_TX8_SLOTS * NUM_TX8_BUFS * BUF_SIZE_BYTES) \
                                                    + (NUM_RX8_SLOTS * NUM_RX8_BUFS * BUF_SIZE_BYTES) \
                                                    + (NUM_TX1_SLOTS * NUM_TX1_BUFS * BUF_SIZE_BYTES) )
#endif


#if (ECHO_TEST_TX8_to_RX1 && ECHO_TEST_TX1_to_RX8)
    #error "Can't conduct ECHO TEST."
#endif

/* To conduct echo test connect mcasp8_axr0 to mcasp1_axr14*/
#if ( ECHO_TEST_TX8_to_RX1 && ( (!MCASP8_TX) || (!MCASP1_RX) || (!MCASP1_TX) ))
    #error "Can't conduct ECHO TEST."
#endif

#if ( ECHO_TEST_TX1_to_RX8 && ( (!MCASP8_TX) || (!MCASP8_RX) || (!MCASP1_TX) ))
    #error "Can't conduct ECHO TEST."
#endif

#if ((NUM_RX1_SLOTS > 32)||(NUM_RX8_SLOTS > 32))
    #error "Read the fucking manual. RXTDMSLOT apparently."
#endif

void clk_cfg (void);

/*
 * 2019.02.04 Conclusion.
 * One PRU can handle only one McASP module.
 * All abstractions are impossible, cause of stack size.
 */

#endif
