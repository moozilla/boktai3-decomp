// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* AnimatedParticleManager_ClearSingleton: Unconditionally zeroes gAnimatedParticleManager, the AnimatedParticleManager singleton pointer set by AnimatedParticleManager_Initialize (080153B0) and read by A */
#define AnimatedParticleManager_ClearSingleton sub_08015154

void sub_08015154(void)
{
    gAnimatedParticleManager = 0;
}
