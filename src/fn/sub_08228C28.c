// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetSunsetHour: Returns the hour stored by the sunset setter */
#define SunClock_GetSunsetHour sub_08228C28

extern u8 gUnk_03005430[];

u8 sub_08228C28(void)
{
    return gUnk_03005430[12];
}
