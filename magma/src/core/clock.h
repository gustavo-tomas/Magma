#ifndef CLOCK_H
#define CLOCK_H

/**
 * @file clock.h
 * @brief Relógio contador de tempo (de acordo com o medidor de tempo da plataforma).
 */

#include "../defines.h"

typedef struct clock
{
    f64 start_time;
    f64 elapsed_time;
} clock;

void update_clock(clock* clock);
void start_clock(clock* clock);
void stop_clock(clock* clock);

#endif // CLOCK_H
