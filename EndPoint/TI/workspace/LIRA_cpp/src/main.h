#ifndef __MAIN_H__
#define __MAIN_H__

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadgetfs.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <wchar.h>
#include <poll.h>
#include <math.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <list>
#include <csignal>

#include "debug.h"

#include "descriptors.h"
#include "mcasp_class.hpp"
#include "usb_gadget_class.hpp"
#include "time_manip.hpp"
#include "upsampler_class.hpp"
#include "downsampler_class.hpp"

#define FETCH(endpoint_descriptor)                                      \
    memcpy(cfg_ptr, &endpoint_descriptor, endpoint_descriptor.bLength); \
    cfg_ptr += endpoint_descriptor.bLength;

#define SELECT_TIMEOUT_MCSEC    (__suseconds_t)(10 * 1000)
// Specific to controller
#define USB_DEV             "/dev/gadget/musb-hdrc"
#define USB_EPIN            "/dev/gadget/ep1in"
#define USB_EPOUT           "/dev/gadget/ep2out"

#define RESAMPLE_8k_to_16k  false
#define C_STYLE             false
#define MCASP_BLK_WR        false

#define TEST_MALLOC(ptr)                                            \
        if (!ptr){                                                  \
            fprintf(stderr, RED"%s : malloc.\r\n"                   \
                    "'errno' : %i. ERROR : %s\n"                    \
                    NORM, __FUNCTION__, errno, strerror(errno));    \
           exit(EXIT_FAILURE);                                      \
        };

struct io_thread_args {
    unsigned stop;
    int      to_dev_ep_fd;
    int      to_hst_ep_fd;
};


#endif
