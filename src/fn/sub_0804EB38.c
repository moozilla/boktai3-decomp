#include "global.h"
#include "engine.h"

s32 SunClock_GetMoonPhase(void);

s32 AreaEffect_GetInitialParticleMode(void)
{
    s32 r;
    if ((gEngineFlags & 0x100) || SunClock_GetMoonPhase() != 4)
        r = 0;
    else
        r = 1;
    return r;
}
