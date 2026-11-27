#ifndef GUARD_INCLUDE_SYSTEM_CLOCK_SOLAR_TIME_CONTEXT_H
#define GUARD_INCLUDE_SYSTEM_CLOCK_SOLAR_TIME_CONTEXT_H

#include "global.h"

struct SolarTimeContext {
    double d[12];
    s32 n[4];
};

typedef char SolarTimeContextSizeCheck[sizeof(struct SolarTimeContext) == 0x70 ? 1 : -1];

#endif
