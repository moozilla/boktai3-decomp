// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetSunriseHour: Returns the hour stored by the sunrise setter */

extern u8 gSunClock[];

u8 SunClock_GetSunriseHour(void)
{
    return gSunClock[10];
}
