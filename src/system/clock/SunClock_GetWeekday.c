// COMPILER: old_agbcc
#include "system/clock/sun_clock.h"
/* SunClock_GetWeekday: Returns the weekday index computed when the date changes */

u8 SunClock_GetWeekday(void)
{
    return gSunClock.field8;
}
