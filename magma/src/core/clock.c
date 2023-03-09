#include "clock.h"

#include "../platform/platform.h"

void update_clock(clock* clock)
{
    if (clock->start_time != 0)
        clock->elapsed_time = platform_get_absolute_time() - clock->start_time;
}

void start_clock(clock* clock)
{
    clock->start_time = platform_get_absolute_time();
    clock->elapsed_time = 0;
}

void stop_clock(clock* clock)
{
    clock->start_time = 0;
}
