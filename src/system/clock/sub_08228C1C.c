// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetSunriseMinute: Returns the minute stored by the sunrise setter */
#define SunClock_GetSunriseMinute sub_08228C1C

extern u8 gSunClock[];

u8 sub_08228C1C(void)
{
    return gSunClock[11];
}
