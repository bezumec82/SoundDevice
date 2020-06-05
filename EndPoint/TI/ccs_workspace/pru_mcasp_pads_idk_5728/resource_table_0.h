#ifndef __RESOURCE_TABLE_0__
#define __RESOURCE_TABLE_0__

#include <stddef.h>
#include <rsc_types.h>
#include <pru_virtio_ids.h>
#include "rpmsg.h"

/* The feature bitmap for virtio rpmsg */
#define VIRTIO_RPMSG_F_NS       0       //name service notifications
/* This firmware supports name service notifications as one of its features */
#define RPMSG_PRU_C0_FEATURES   (1 << VIRTIO_RPMSG_F_NS)
/*
 *  Sizes of the virtqueues (expressed in number of buffers supported,
 * and must be power of 2)
 */
#define PRU_RPMSG_VQ0_SIZE  16
#define PRU_RPMSG_VQ1_SIZE  16
/* Definition for unused interrupts */
#define HOST_UNUSED         255
/* Mapping sysevts to a channel. Each pair contains a sysevt, channel. */

#if     (PRU0_MCASP1)
struct ch_map pru_intc_map[] =
{
    {
        .evt   = 16,
        .ch    = 2
    },
    {
        .evt   = 17,
        .ch    = 0
    },
};
#elif   (PRU1_MCASP2)
struct ch_map pru_intc_map[] =
{
    {
        .evt   = 18,
        .ch    = 3
    },
    {
        .evt   = 19,
        .ch    = 1
    },
};
#endif

struct my_resource_table {
    struct resource_table       base;
    uint32_t                    offset[2]; /* Should match 'num' in actual definition */
    /* rpmsg vdev entry */
    struct fw_rsc_vdev          rpmsg_vdev;
    struct fw_rsc_vdev_vring    rpmsg_vring0;
    struct fw_rsc_vdev_vring    rpmsg_vring1;
    /* intc definition */
    struct fw_rsc_custom        pru_ints;
};

#pragma DATA_SECTION(resourceTable, ".resource_table")
#pragma RETAIN(resourceTable)
struct my_resource_table resourceTable = {
    .base.ver = 1,                  /* Resource table version: only version 1 is supported by the current driver */
    .base.num = 2,                  /* number of entries in the table */
    .base.reserved = { 0, 0 },      /* reserved, must be zero */
    .offset =
    {
         offsetof(struct my_resource_table, rpmsg_vdev),
         offsetof(struct my_resource_table, pru_ints),
    },
    /* rpmsg vdev entry */
    .rpmsg_vdev =
    {
        .type           = (uint32_t)    TYPE_VDEV,              //type
        .id             = (uint32_t)    VIRTIO_ID_RPMSG,        //id
        .notifyid       = (uint32_t)    0,                      //notifyid
        .dfeatures      = (uint32_t)    RPMSG_PRU_C0_FEATURES,  //dfeatures
        .gfeatures      = (uint32_t)    0,                      //gfeatures
        .config_len     = (uint32_t)    0,                      //config_len
        .status         = (uint8_t)     0,                      //status
        .num_of_vrings  = (uint8_t)     2,                      //num_of_vrings, only two is supported
        .reserved       = { (uint8_t)0, (uint8_t)0 },           //reserved
        /* no config data */
    },
    /* the two vrings */
    .rpmsg_vring0 =
    {
        .da         = (uint32_t) 0,                             //da, will be populated by host, can't pass it in
        .align      = (uint32_t) 16,                            //align (bytes),
        .num        = (uint32_t) PRU_RPMSG_VQ0_SIZE,            //num of descriptors
        .notifyid   = (uint32_t) 0,                             //notifyid, will be populated, can't pass right now
        .reserved   = (uint32_t) 0                              //reserved
    },
    .rpmsg_vring1 =
    {
        .da         = (uint32_t) 0,                             //da, will be populated by host, can't pass it in
        .align      = (uint32_t) 16,                            //align (bytes),
        .num        = (uint32_t) PRU_RPMSG_VQ1_SIZE,            //num of descriptors
        .notifyid   = (uint32_t) 0,                             //notifyid, will be populated, can't pass right now
        .reserved   = (uint32_t) 0                              //reserved
    },
    .pru_ints =
    {
        .type       = TYPE_POSTLOAD_VENDOR,
        .u.sub_type = PRU_INTS_VER0 | TYPE_PRU_INTS,
        .rsc_size   = sizeof(struct fw_rsc_custom_ints),
        .rsc = /* union fw_custom */
        {
            .reserved       = 0x0000,
            /* Channel-to-host mapping, 255 for unused */
            .pru_ints = /* struct fw_rsc_custom_ints */
            {
                 .channel_host = /* Channel-to-host mapping (10 units), 255 for unused */
            #if     (PRU0_MCASP1)

                {
                     0,             //0
                     HOST_UNUSED,   //1
                     2,             //2
                     HOST_UNUSED,   //3
                     HOST_UNUSED,   //4
                     HOST_UNUSED,   //5
                     HOST_UNUSED,   //6
                     HOST_UNUSED,   //7
                     HOST_UNUSED,   //8
                     HOST_UNUSED    //9
                },
            #elif   (PRU1_MCASP2)
                {
                     HOST_UNUSED,   //0
                     1,             //1
                     HOST_UNUSED,   //2
                     3,             //3
                     HOST_UNUSED,   //4
                     HOST_UNUSED,   //5
                     HOST_UNUSED,   //6
                     HOST_UNUSED,   //7
                     HOST_UNUSED,   //8
                     HOST_UNUSED    //9
                }
            #endif
                /* Number of evts being mapped to channels */
                .num_evts       = (sizeof(pru_intc_map) / sizeof(struct ch_map)),
                /* Pointer to the structure containing mapped events */
                .event_channel  = pru_intc_map, /* 'struct ch_map *' */
            }, /* struct fw_rsc_custom pru_ints -> struct fw_rsc_custom -> union fw_custom rsc -> struct fw_rsc_custom_ints pru_ints */
        }, /* struct fw_rsc_custom pru_ints -> struct fw_rsc_custom -> union fw_custom rsc */
    }, /* struct fw_rsc_custom pru_ints */

};

#endif /* _RSC_TABLE_PRU_H_ */
