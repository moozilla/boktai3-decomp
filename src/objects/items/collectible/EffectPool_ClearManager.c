// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* EffectPool_ClearManager: Clears the singleton effect manager pointer */

void EffectPool_ClearManager(void)
{
    gParticleManager = 0;
}
