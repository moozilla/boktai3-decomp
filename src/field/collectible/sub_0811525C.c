// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/ram.h"
/* EffectPool_ClearManager: Clears the singleton effect manager pointer */
#define EffectPool_ClearManager sub_0811525C

void sub_0811525C(void)
{
    gParticleManager = 0;
}
