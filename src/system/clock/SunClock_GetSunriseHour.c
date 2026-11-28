// COMPILER: old_agbcc
#include "system/clock/sun_clock.h"
/* SunClock_GetSunriseHour: Returns the hour stored by the sunrise setter */

u8 SunClock_GetSunriseHour(void)
{
    return gSunClock.hourA;
}
