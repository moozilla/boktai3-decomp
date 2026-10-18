#include "global.h"

struct S {
    u8 filler[0x1c];
    u16 v;
};
s32 SunClock_GetPhase(void);

s32 AreaEffect_TestParticleMode(struct S *p)
{
    if (p->v != 0) {
        if ((u32)(SunClock_GetPhase() - 1) > 2)
            return 1;
    }
    return 0;
}
