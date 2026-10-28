// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetMoonPhaseIndex: Returns the same cached lunar phase byte through a second accessor */
#define SunClock_GetMoonPhaseIndex sub_082290E8

extern u8 gSunClock[];

u8 sub_082290E8(void)
{
    return gSunClock[22];
}
