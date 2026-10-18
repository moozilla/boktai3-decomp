// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetSunriseMinute: Returns the minute stored by the sunrise setter */
#define SunClock_GetSunriseMinute sub_08228C1C

extern u8 gUnk_03005430[];

u8 sub_08228C1C(void)
{
    return gUnk_03005430[11];
}
