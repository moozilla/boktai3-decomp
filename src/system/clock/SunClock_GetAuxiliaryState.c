// COMPILER: old_agbcc
#include "system/clock/sun_clock.h"
/* SunClock_GetAuxiliaryState: Returns the byte following weekday whose auxiliary role remains tentative */

u8 SunClock_GetAuxiliaryState(void)
{
    return gSunClock.pad9;
}
