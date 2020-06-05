#ifndef __RPMSG__
#define __RPMSG__

/* Host-0 Interrupt sets bit 30 in register R31 */
#define HOST_INT                    ((uint32_t) 1 << 30)

/* The PRU-ICSS system events used for RPMsg are defined in the Linux device tree
 * PRU0 uses system event 16 (To ARM) and 17 (From ARM)
 * PRU1 uses system event 18 (To ARM) and 19 (From ARM)
 */
#define TO_ARM_HOST                 16
#define FROM_ARM_HOST               17

#define CHAN_NAME                   "mcasp_pru0"
#define CHAN_DESC                   "Channel 30"
#define CHAN_PORT                   30

/*
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK   4



#endif