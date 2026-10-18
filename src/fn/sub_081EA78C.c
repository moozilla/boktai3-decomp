#include "global.h"

s32 SunClock_GetPhase(void);

s32 TimeDisplay_GetLightPhaseGroup(void)
{
    s32 r;
    switch (SunClock_GetPhase()) {
        case 1:
        case 2:
        case 3:
            r = 0;
            break;
        case 0:
        case 4:
        case 5:
            r = 1;
            break;
        default:
            r = -1;
            break;
    }
    return r;
}
