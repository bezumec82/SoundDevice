#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>
#include <pru_cfg.h>
#include "cm_regs.h"
#include "gpio_regs.h"
#include "pads_cfg.h"
#include "mcasp_init.h"
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>

#define TRUE                    1
#define FALSE                   0

#define IDK_AM5728              FALSE
#define BBB_AM335x              TRUE
#include "mcasp_regs.h"

#if (IDK_AM5728)
    #define SETUP_DPLL_ABE      TRUE
#endif

#if (BBB_AM335x)
    #define SETUP_DPLL          FALSE
    #define CLK_CFG             FALSE
#endif
#define PRU0                    TRUE
#define PRU1                    FALSE
#define MCASP0                  TRUE
#define MCASP1                  FALSE
#if (IDK_AM5728)
    #define MCASP8              FALSE
#endif

#define OPTIMIZED               TRUE
#define RPC_MSG                 TRUE
#define CDC_IS_MASTER           TRUE
#define CRISS_CROSS             TRUE

#if     (IDK_AM5728 & BBB_AM335x)
        #error "Wrong configuration"
#endif
#if     (IDK_AM5728)
    #if ((MCASP1 == MCASP8)
        #error "Wrong configuration"
    #endif
#endif
#if     (BBB_AM335x)
    #if (MCASP0 == MCASP1)
        #error "Wrong configuration"
    #endif
#endif
#if     (PRU0 == PRU1)
        #error "Wrong configuration"
#endif

#if     (PRU0)&&(MCASP0)
    #define MCASP_CFG_ADDR                  MCASP0_CFG_ADDR
    #define CHAN_NAME                       "mcasp0_pru0"
#elif   (PRU0)&&(MCASP1)
    #define MCASP_CFG_ADDR                  MCASP1_CFG_ADDR
    #define CHAN_NAME                       "mcasp1_pru0"
#elif   (PRU1)&&(MCASP0)
    #define MCASP_CFG_ADDR                  MCASP0_CFG_ADDR
    #define CHAN_NAME                       "mcasp10_pru1"
#elif   (PRU1)&&(MCASP1)
    #define MCASP_CFG_ADDR                  MCASP1_CFG_ADDR
    #define CHAN_NAME                       "mcasp1_pru1"
    #if (IDK_AM5728)
        #elif   (PRU0)&&(MCASP8)
            #define MCASP_CFG_ADDR          MCASP8_CFG_ADDR
            #define CHAN_NAME               "mcasp8_pru0"
        #elif   (PRU1)&(MCASP8)
            #define MCASP_CFG_ADDR          MCASP8_CFG_ADDR
            #define CHAN_NAME               "mcasp8_pru1"
    #endif
#else
    #error "Wrong configuration"
#endif

/* The PRU-ICSS system events used for RPMsg
 * are defined in the Linux device tree
 * PRU0 uses system event 16 (To ARM) and 17 (From ARM)
 * PRU1 uses system event 18 (To ARM) and 19 (From ARM)
 */
#if     (PRU0)
    #define CHAN_DESC                   "Channel 30"
    #define CHAN_PORT                   30
    #define TO_ARM_HOST                 16
    #define FROM_ARM_HOST               17
    /* Host-0 Interrupt sets bit 30 in register R31 */
    #define HOST_INT                    ((uint32_t) 1 << 30)
#elif   (PRU1)
    #define CHAN_DESC                   "Channel 31"
    #define CHAN_PORT                   31
    #define TO_ARM_HOST                 18
    #define FROM_ARM_HOST               19
    /* Host-1 Interrupt sets bit 31 in register R31 */
    #define HOST_INT                    ((uint32_t) 1 << 31)
#endif

/*
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK       4

#define MCASP_TX                        TRUE
#define MCASP_RX                        TRUE

#define PAGE_ALIGNED_BUFS               TRUE

#define DEBUG_BUF                       FALSE   //TX output will show numbers
#define DEBUG_RUNTIME_TX                FALSE   //The same, but numbers will fill dynamically
#define DEBUG_MCASP_ERRS                FALSE

/*
 * Packages at upper level arrives in 20ms size.
 * We should have possibility place 2 packages ahead.
 * 1 buf = 64 words == 4 ms (16kHz = 16 words per ms)
 * 2 * 20 + 4 = 44 ms == 11 bufs (buf in work and two 20ms frames ahead)
 */
#define BUF_SIZE_WORDS          64
#define BUF_SIZE_BYTES          BUF_SIZE_WORDS * 4
#define PAGE_SIZE               4096

#define NUM_TX_SLOTS            2
#define NUM_RX_SLOTS            2
#if (NUM_RX_SLOTS > 32)
    #error "Read the fucking manual. RXTDMSLOT apparently."
#endif

#define NUM_TX_BUFS             16
#define NUM_RX_BUFS             16

#define RX_BUF_BASE_OFFSET   (uint32_t)( NUM_TX_SLOTS * NUM_TX_BUFS * PAGE_SIZE )

void clk_cfg (void);

#if (IDK_AM5728)
    #define DEBUG_LEDS_IDK_IDK_AM5728   TRUE
    #if (DEBUG_LEDS_IDK_IDK_AM5728)
        void setup_gpio_pads(void);

        void set_status_0_green     (void);
        void set_status_1_green     (void);
        void set_industial_2_green  (void);
        void set_industial_3_green  (void);

        void set_status_0_red       (void);
        void set_status_1_red       (void);
        void set_industial_2_red    (void);
        void set_industial_3_red    (void);

        void set_status_0_blue      (void);
        void set_status_1_blue      (void);
        void set_industial_2_blue   (void);
        void set_industial_3_blue   (void);
    #endif
#endif


#endif /* __MAIN_H__ */
