#include <thread>
#include <queue>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

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
#include <time.h>

#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadgetfs.h>

#include "debug.h"
#include "descriptors.h"
#include "suplemental.hpp"

#define PAGE_SIZE           4096
#define MAX_PCKGS_IN_QUEUE  5   /* 5 by 20ms == 100ms */
#define AUDIO_QUANTUM_MS    20
#define PCKGS_PER_SECOND    (1000/AUDIO_QUANTUM_MS)
#define RD_STAT_MAX_SIZE    (10 * PCKGS_PER_SECOND)

#define DEBUG_READ_STAT     true

#ifndef PRINTF
#define PRINTF PRINTF(color, text, ...)
#define PRINT_ERR(text, ...)
#endif

class usb_gadget
{
private:
    std::string usb_dev_name           = "/dev/gadget/musb-hdrc";
    std::string usb_to_hst_ep_dev_name = "/dev/gadget/ep1in";
    std::string usb_to_dev_ep_dev_name = "/dev/gadget/ep2out";

    std::thread ep0_thread;
    uint8_t stop_ep0_thrd;

    std::thread read_thread;
    uint8_t stop_read_thrd;

    int ep0_fd;
    int to_hst_ep_fd;
    int to_dev_ep_fd;

public:
    usb_gadget(void);
    ~usb_gadget(void);

    int cfg_ep0(void);
    int cfg_rd_wr_ep(void);

    int handle_ep0(void);

    void handle_ep0_thread_func(void);
    void read_thread_func(void);

    void handle_ep0_events  (
                            struct usb_gadgetfs_event * event_ptr,
                            int events_ammnt
                            );
    void handle_setup_request(struct usb_ctrlrequest * setup_ptr);

    std::queue  < std::pair < uint8_t * ,       size_t > > recv_vec; /* To pass vectored log to another class */
    std::vector < std::pair < struct timespec , size_t > > recv_stat_vec;
    uint32_t recv_bytes_mov_avrg     = 0;
};
