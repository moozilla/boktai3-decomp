// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* ParticleBurstEffect_ResetManager: Clears the six-slot effect manager singleton pointer */
#define ParticleBurstEffect_ResetManager sub_08139FA0

void sub_08139FA0(void)
{
    gSceneProcParticles = 0;
}
