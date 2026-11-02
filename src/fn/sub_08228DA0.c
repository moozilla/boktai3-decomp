#include "sun_clock.h"

u32 SunClock_GetSecond(void)
{
    return gSunClock.extra6;
}
