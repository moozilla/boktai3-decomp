// COMPILER: old_agbcc
#include "global.h"
/* SunClock_GetWeekday: Returns the weekday index computed when the date changes */
#define SunClock_GetWeekday sub_08228DAC

extern u8 gUnk_03005430[];

u8 sub_08228DAC(void)
{
    return gUnk_03005430[8];
}
