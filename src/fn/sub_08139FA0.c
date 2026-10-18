// COMPILER: old_agbcc
#include "global.h"
/* ParticleBurstEffect_ResetManager: Clears the six-slot effect manager singleton pointer */
#define ParticleBurstEffect_ResetManager sub_08139FA0

extern u32 gUnk_020004D0;

void sub_08139FA0(void)
{
    gUnk_020004D0 = 0;
}
