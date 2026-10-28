// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetSunsetMinute: Returns the minute stored by the sunset setter */
#define SunClock_GetSunsetMinute sub_08228C34

extern u8 gSunClock[];

u8 sub_08228C34(void)
{
    return gSunClock[13];
}
