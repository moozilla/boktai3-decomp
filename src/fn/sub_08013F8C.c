// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* ParticleManager_ClearTaskPtr: zeroes the ParticleManager singleton pointer gParticleGroupManager, same global ParticleManager_Kill clears on destroy */
#define ParticleManager_ClearTaskPtr sub_08013F8C

void sub_08013F8C(void)
{
    gParticleGroupManager = 0;
}
