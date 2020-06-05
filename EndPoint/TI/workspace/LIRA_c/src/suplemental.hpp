#ifndef __SUPLEMENTAL_HPP__
#define __SUPLEMENTAL_HPP__

#include "time.h"

double Get_timestamp_ms(void);
double timespec_to_ms(struct timespec& tspec);
double timespecs_to_delta    (
                                    struct timespec& tspec_start,
                                    struct timespec& tspec_end
                                    );

#endif
