#include "global.h"

extern u32 gUnk_020001E0;
void SpriteParticle_LoadAnimationData(u32);

s32 SpriteParticle_InitManager(u32 a)
{
    SpriteParticle_LoadAnimationData(a);
    gUnk_020001E0 = a;
    return 0;
}
