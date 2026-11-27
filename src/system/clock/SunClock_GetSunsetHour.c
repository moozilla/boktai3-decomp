// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetSunsetHour: Returns the hour stored by the sunset setter */

extern u8 gSunClock[];

u8 SunClock_GetSunsetHour(void)
{
    return gSunClock[12];
}
