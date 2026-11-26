// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetMoonPhaseIndex: Returns the same cached lunar phase byte through a second accessor */

extern u8 gSunClock[];

u8 SunClock_GetMoonPhaseIndex(void)
{
    return gSunClock[22];
}
