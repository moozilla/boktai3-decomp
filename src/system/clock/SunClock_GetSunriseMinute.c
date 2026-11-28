// COMPILER: old_agbcc
#include "system/clock/sun_clock.h"
/* SunClock_GetSunriseMinute: Returns the minute stored by the sunrise setter */

u8 SunClock_GetSunriseMinute(void)
{
    return gSunClock.minuteA;
}
