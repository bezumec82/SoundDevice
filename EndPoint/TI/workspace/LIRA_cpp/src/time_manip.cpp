#include "time_manip.hpp"

double Get_timestamp_ms(void)
{
    struct timespec tspec;
    clock_gettime(CLOCK_REALTIME, &tspec);
    return ( round(double(tspec.tv_nsec) / 1.0e6) + double( tspec.tv_sec) * 1.0e3 ) ;
}

double timespec_to_millisecs(struct timespec& tspec)
{
    return (double(tspec.tv_nsec) / 1.0e6 + double(tspec.tv_sec) * 1.0e3);
}



double timespec_to_nanosecs(struct timespec& tspec)
{
    return (double(tspec.tv_nsec) + double(tspec.tv_sec) * 1.0e9);
}

double timespec_to_microsecs(struct timespec& tspec)
{
    return (double(tspec.tv_nsec) / 1.0e3 + double(tspec.tv_sec) * 1.0e6);
}

double timespecs_to_delta   (
                            struct timespec& tspec_start,
                            struct timespec& tspec_end
                            )
{
    return double(timespec_to_microsecs(tspec_end) - timespec_to_microsecs(tspec_start) );
}
