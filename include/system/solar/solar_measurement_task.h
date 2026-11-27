#ifndef GUARD_INCLUDE_SYSTEM_SOLAR_SOLAR_MEASUREMENT_TASK_H
#define GUARD_INCLUDE_SYSTEM_SOLAR_SOLAR_MEASUREMENT_TASK_H

#include "global.h"

struct SolarMeasurementTask {
    u8 pad0[0x19];
    u8 state;
    u16 pad1a; /* confidence=low */
    s16 a;
    u16 gain; /* confidence=low */
    u16 count;
    u8 pad22[0x2];
    u16 frame;
    u16 idle;
    u16 frac;
    u8 pad2A[0x2];
};

typedef char SolarMeasurementTaskSizeCheck[sizeof(struct SolarMeasurementTask) == 0x2c ? 1 : -1];

#endif
