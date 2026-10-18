// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetSunriseHour: Returns the hour stored by the sunrise setter */
#define SunClock_GetSunriseHour sub_08228C10

extern u8 gUnk_03005430[];

u8 sub_08228C10(void)
{
    return gUnk_03005430[10];
}
