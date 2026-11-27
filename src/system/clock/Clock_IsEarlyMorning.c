#include "global.h"
/* Clock_IsEarlyMorning: Tests whether the cached base hour is between midnight and two inclusive */
extern u8 gSunClock[];
u32 Clock_IsEarlyMorning(void)
{
    if (gSunClock[4] <= 2)
        return 1;
    return 0;
}
