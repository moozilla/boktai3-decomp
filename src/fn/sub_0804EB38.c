#include "global.h"

extern u32 gUnk_030053F4;
s32 SunClock_GetMoonPhase(void);

s32 AreaEffect_GetInitialParticleMode(void)
{
    s32 r;
    if ((gUnk_030053F4 & 0x100) || SunClock_GetMoonPhase() != 4)
        r = 0;
    else
        r = 1;
    return r;
}
