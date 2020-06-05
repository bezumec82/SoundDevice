#ifndef __RPMSG__
#define __RPMSG__

#include "main.h"



/* The PRU-ICSS system events used for RPMsg are defined in the Linux device tree
 * PRU0 uses system event 16 (To ARM) and 17 (From ARM)
 * PRU1 uses system event 18 (To ARM) and 19 (From ARM)
 */


#if     (PRU0_MCASP1)

    #define CHAN_NAME                   "mcasp1_pru0"
    #define CHAN_DESC                   "Channel 30"
    #define CHAN_PORT                   30

    #define TO_ARM_HOST                 16
    #define FROM_ARM_HOST               17

    /* Host-0 Interrupt sets bit 30 in register R31 */
    #define HOST_INT                    ((uint32_t) 1 << 30)

#elif   (PRU1_MCASP2)

    #define CHAN_NAME                   "mcasp2_pru1"
    #define CHAN_DESC                   "Channel 31"
    #define CHAN_PORT                   31

    #define TO_ARM_HOST                 18
    #define FROM_ARM_HOST               19

    /* Host-1 Interrupt sets bit 31 in register R31 */
    #define HOST_INT            ((uint32_t) 1 << 31)

#endif

/*
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK       4



#endif
