#include "global.h"

extern u32 gUnk_020001DC;
void NativeParticle_LoadResources(u32);

s32 EffectPool_InitManager(u32 a)
{
    NativeParticle_LoadResources(a);
    gUnk_020001DC = a;
    return 0;
}
