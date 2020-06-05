#include "suplemental.hpp"
#include <math.h>

double Get_timestamp_ms(void)
{
    struct timespec tspec;
    clock_gettime(CLOCK_REALTIME, &tspec);
    return ( round(double(tspec.tv_nsec) / 1.0e6) + double( tspec.tv_sec) * 1000.0 ) ;
}

double timespec_to_ms(struct timespec& tspec)
{
    return (double(tspec.tv_nsec) / 1.0e6 + double(tspec.tv_sec) * 1000.0);
}

double timespecs_to_delta  (
                            struct timespec& tspec_start,
                            struct timespec& tspec_end
                            )
{
    return double(timespec_to_ms(tspec_end) - timespec_to_ms(tspec_start) );
}
