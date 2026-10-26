#include "global.h"
#include "ram.h"

void NativeParticle_LoadResources(u32);

s32 EffectPool_InitManager(u32 a)
{
    NativeParticle_LoadResources(a);
    gParticleManager = a;
    return 0;
}
