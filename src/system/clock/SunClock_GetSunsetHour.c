// COMPILER: old_agbcc
#include "system/clock/sun_clock.h"
/* SunClock_GetSunsetHour: Returns the hour stored by the sunset setter */

u8 SunClock_GetSunsetHour(void)
{
    return gSunClock.hourB;
}
