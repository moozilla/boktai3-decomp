#include "system/clock/sun_clock.h"
/* Clock_IsEarlyMorning: Tests whether the cached base hour is between midnight and two inclusive */
u32 Clock_IsEarlyMorning(void)
{
    if (gSunClock.baseHour <= 2)
        return 1;
    return 0;
}
