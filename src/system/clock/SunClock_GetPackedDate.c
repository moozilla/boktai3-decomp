#include "system/clock/sun_clock.h"

u32 SunClock_GetPackedDate(void)
{
    return gSunClock.stamp;
}
