#include "sun_clock.h"
#define SunClock_GetBaseHour sub_08228D88
u32 SunClock_GetBaseHour(void)
{
    return gUnk_03005430.baseHour;
}
