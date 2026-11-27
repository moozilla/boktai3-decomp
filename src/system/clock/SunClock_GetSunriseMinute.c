// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetSunriseMinute: Returns the minute stored by the sunrise setter */

extern u8 gSunClock[];

u8 SunClock_GetSunriseMinute(void)
{
    return gSunClock[11];
}
