// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetSunriseHour: Returns the hour stored by the sunrise setter */
#define SunClock_GetSunriseHour sub_08228C10

extern u8 gSunClock[];

u8 sub_08228C10(void)
{
    return gSunClock[10];
}
