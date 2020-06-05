#include "main.h"

int poll_all (int ep0_fd)
{
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(ep0_fd, &read_set);
    select(ep0_fd + 1,
           &read_set,
           NULL,
           NULL,
           NULL);

return 0;
}
