// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetAuxiliaryState: Returns the byte following weekday whose auxiliary role remains tentative */

extern u8 gSunClock[];

u8 SunClock_GetAuxiliaryState(void)
{
    return gSunClock[9];
}
