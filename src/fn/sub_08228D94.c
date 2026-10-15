#include "sun_clock.h"
#define SunClock_GetBaseMinute sub_08228D94
u32 SunClock_GetBaseMinute(void)
{
    return gUnk_03005430.baseMinute;
}
