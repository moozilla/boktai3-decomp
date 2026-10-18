// COMPILER: old_agbcc
#include "global.h"
/* AnimatedParticleManager_ClearSingleton: Unconditionally zeroes gUnk_02000068, the AnimatedParticleManager singleton pointer set by AnimatedParticleManager_Initialize (080153B0) and read by A */
#define AnimatedParticleManager_ClearSingleton sub_08015154

extern u32 gUnk_02000068;

void sub_08015154(void)
{
    gUnk_02000068 = 0;
}
