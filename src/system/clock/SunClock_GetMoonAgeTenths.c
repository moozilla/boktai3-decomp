// COMPILER: old_agbcc
#include "system/clock/sun_clock.h"
/* SunClock_GetMoonAgeTenths: Returns the cached lunar age stored in tenths of a day */

u16 SunClock_GetMoonAgeTenths(void)
{
    return gSunClock.moonAgeTenths;
}
