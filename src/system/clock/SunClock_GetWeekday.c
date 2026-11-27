// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetWeekday: Returns the weekday index computed when the date changes */

extern u8 gSunClock[];

u8 SunClock_GetWeekday(void)
{
    return gSunClock[8];
}
