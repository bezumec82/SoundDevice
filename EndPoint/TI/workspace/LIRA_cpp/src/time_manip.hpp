#ifndef __TIME_MANIP_HPP__
#define __TIME_MANIP_HPP__

#include <time.h>
#include <math.h>

double Get_timestamp_ms(void);
double timespec_to_millisecs(struct timespec& tspec);
double timespec_to_microsecs(struct timespec& tspec);
double timespec_to_nanosecs(struct timespec& tspec);
double timespecs_to_delta    (
                                    struct timespec& tspec_start,
                                    struct timespec& tspec_end
                                    );

#endif
