// COMPILER: old_agbcc
#include "system/clock/sun_clock.h"
/* SunClock_GetSunsetMinute: Returns the minute stored by the sunset setter */

u8 SunClock_GetSunsetMinute(void)
{
    return gSunClock.minuteB;
}
