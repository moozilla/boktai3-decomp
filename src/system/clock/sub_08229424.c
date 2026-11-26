#include "global.h"
/* Clock_IsEarlyMorning: Tests whether the cached base hour is between midnight and two inclusive */
#define Clock_IsEarlyMorning sub_08229424
extern u8 gSunClock[];
u32 sub_08229424(void)
{
    if (gSunClock[4] <= 2)
        return 1;
    return 0;
}
