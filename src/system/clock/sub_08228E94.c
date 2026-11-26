// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetAuxiliaryState: Returns the byte following weekday whose auxiliary role remains tentative */
#define SunClock_GetAuxiliaryState sub_08228E94

extern u8 gSunClock[];

u8 sub_08228E94(void)
{
    return gSunClock[9];
}
