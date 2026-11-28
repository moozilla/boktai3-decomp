// COMPILER: old_agbcc
#include "system/clock/sun_clock.h"
/* SunClock_GetMoonPhaseIndex: Returns the same cached lunar phase byte through a second accessor */

u8 SunClock_GetMoonPhaseIndex(void)
{
    return gSunClock.moonPhase;
}
