#ifndef _VMUX_H_
#define _VMUX_H_

#include "driver.h"

#define MUX_PACKET_FACTOR       1

struct mux_node
{
    struct list_head    node;
    int                 active;
    channel_t *         src;
    channel_t *         dst;
};

int     mux_connect                     (u16 dest, u16 src);
int     mux_disconnect                  (u16 chn);
int     mux_init                        (void *context);
void    mux_deinit                      (void *context);
void    mux_set_kernel_panic_enabled    (int enabled);

#endif /* _VMUX_H_ */
/*--- End of the file ----------------------------------------------------------------------------*/
