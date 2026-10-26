#include "global.h"
#include "ram.h"

void SpriteParticle_LoadAnimationData(u32);

s32 SpriteParticle_InitManager(u32 a)
{
    SpriteParticle_LoadAnimationData(a);
    gScreenMoteManager = a;
    return 0;
}
