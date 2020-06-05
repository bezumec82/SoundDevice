#include "main.h"

long Get_timestamp_ms(void) {
    struct timespec tspec = { 0 , 0 };
    clock_gettime(CLOCK_REALTIME, &tspec);
    return (((long) ((round(tspec.tv_nsec / 1.0e6)) + tspec.tv_sec * 1000)) & 0xffff);
}
/*----------------------------------------------------------------------------*/

#define POLL_FDS_AMMNT                  4
#define PRU0_MCASP0_TS0_POLL_IDX        0
#define PRU0_MCASP0_TS1_POLL_IDX        1
#define PRU1_MCASP1_TS0_POLL_IDX        2
#define PRU1_MCASP1_TS1_POLL_IDX        3

#define POLL_TIMEOUT                    100

#define PRU0_MCASP0                     1
#define PRU1_MCASP1                     1

int main() {
#if (!SINE_TEST)
    int ret_val;
    unsigned long pstart_time_stamp;
    struct pollfd fds[POLL_FDS_AMMNT];
#endif

                                           /* pru#  mcasp#  ts# */
#if (PRU0_MCASP0)
    mcasp_ts * pru0_mcasp0_ts0 = new mcasp_ts(PRU0, MCASP0, TS0);
    mcasp_ts * pru0_mcasp0_ts1 = new mcasp_ts(PRU0, MCASP0, TS1);
#endif
#if (PRU1_MCASP1)
    mcasp_ts * pru1_mcasp1_ts0 = new mcasp_ts(PRU1, MCASP1, TS0);
    mcasp_ts * pru1_mcasp1_ts1 = new mcasp_ts(PRU1, MCASP1, TS1);
#endif

#if (!SINE_TEST)
#if (PRU0_MCASP0)
    fds[PRU0_MCASP0_TS0_POLL_IDX].fd = pru0_mcasp0_ts0->fd;
    fds[PRU0_MCASP0_TS0_POLL_IDX].events = POLLOUT | POLLIN;
    fds[PRU0_MCASP0_TS0_POLL_IDX].revents = 0;

    fds[PRU0_MCASP0_TS1_POLL_IDX].fd = pru0_mcasp0_ts1->fd;
    fds[PRU0_MCASP0_TS1_POLL_IDX].events = POLLOUT | POLLIN;
    fds[PRU0_MCASP0_TS1_POLL_IDX].revents = 0;
#else
    fds[PRU0_MCASP0_TS0_POLL_IDX].fd = -1;
    fds[PRU0_MCASP0_TS1_POLL_IDX].fd = -1;
#endif

#if (PRU1_MCASP1)
    fds[PRU1_MCASP1_TS0_POLL_IDX].fd = pru1_mcasp1_ts0->fd;
    fds[PRU1_MCASP1_TS0_POLL_IDX].events = POLLOUT | POLLIN;
    fds[PRU1_MCASP1_TS0_POLL_IDX].revents = 0;

    fds[PRU1_MCASP1_TS1_POLL_IDX].fd = pru1_mcasp1_ts1->fd;
    fds[PRU1_MCASP1_TS1_POLL_IDX].events = POLLOUT | POLLIN;
    fds[PRU1_MCASP1_TS1_POLL_IDX].revents = 0;
#else
    fds[PRU1_MCASP1_TS0_POLL_IDX].fd = -1;
    fds[PRU1_MCASP1_TS0_POLL_IDX].fd = -1;
#endif
#endif
    while (1)
    {
#if (SINE_TEST)
    #if (PRU0_MCASP0)
        pru0_mcasp0_ts0->write_sin();
        pru0_mcasp0_ts1->write_sin();
    #endif
    #if (PRU1_MCASP1)
        pru1_mcasp1_ts0->write_sin();
        pru1_mcasp1_ts1->write_sin();
    #endif
#else
        pstart_time_stamp = Get_timestamp_ms();
        ret_val = poll  (
                        fds,            /* The set of file descriptors to be monitored. */
                        POLL_FDS_AMMNT, /* The number of items in the array of 'struct pollfd' type. */
                        POLL_TIMEOUT    /* The number of milliseconds that 'poll()' should block waiting for a file descriptor to become ready. */
                        );
        if (ret_val == -1)
        {
            FPRINTF;
        }
        if (!ret_val)
        {
            PRINT_ERR("Timeout!.\r\n"
                    "%49s Elapsed %lums.\r\n",
                    "", (Get_timestamp_ms() - pstart_time_stamp));
            continue;
        }
        if (fds[PRU1_MCASP1_TS0_POLL_IDX].revents & POLLIN)
        {
            pru1_mcasp1_ts0->read_mcasp     ( pru1_mcasp1_ts0->recv_buf, pru1_mcasp1_ts0->recv_buf_byte_size );
        }
        if (fds[PRU1_MCASP1_TS1_POLL_IDX].revents & POLLIN)
        {
            pru1_mcasp1_ts1->read_mcasp     ( pru1_mcasp1_ts1->recv_buf, pru1_mcasp1_ts1->recv_buf_byte_size );
        }

        if (fds[PRU0_MCASP0_TS0_POLL_IDX].revents & POLLOUT)
        {
            pru0_mcasp0_ts0->write_mcasp    ( pru1_mcasp1_ts0->recv_buf, pru1_mcasp1_ts0->recv_buf_byte_size );
        }
        if (fds[PRU0_MCASP0_TS1_POLL_IDX].revents & POLLOUT)
        {
            pru0_mcasp0_ts1->write_mcasp    ( pru1_mcasp1_ts1->recv_buf, pru1_mcasp1_ts1->recv_buf_byte_size );
        }

        if (fds[PRU0_MCASP0_TS0_POLL_IDX].revents & POLLIN)
        {
            pru0_mcasp0_ts0->read_mcasp     ( pru0_mcasp0_ts0->recv_buf, pru0_mcasp0_ts0->recv_buf_byte_size );
        }
        if (fds[PRU0_MCASP0_TS1_POLL_IDX].revents & POLLIN)
        {
            pru0_mcasp0_ts1->read_mcasp     ( pru0_mcasp0_ts1->recv_buf, pru0_mcasp0_ts1->recv_buf_byte_size );
        }

        if (fds[PRU1_MCASP1_TS0_POLL_IDX].revents & POLLOUT)
        {
            pru1_mcasp1_ts0->write_mcasp    ( pru0_mcasp0_ts0->recv_buf, pru0_mcasp0_ts0->recv_buf_byte_size );
        }
        if (fds[PRU1_MCASP1_TS1_POLL_IDX].revents & POLLOUT)
        {
            pru1_mcasp1_ts1->write_mcasp    ( pru0_mcasp0_ts1->recv_buf, pru0_mcasp0_ts1->recv_buf_byte_size );
        }
#endif
    } //end while (1)

	return 0;
}
