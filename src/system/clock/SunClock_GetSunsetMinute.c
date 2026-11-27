// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetSunsetMinute: Returns the minute stored by the sunset setter */

extern u8 gSunClock[];

u8 SunClock_GetSunsetMinute(void)
{
    return gSunClock[13];
}
