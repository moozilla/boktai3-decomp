#include "global.h"
#include "ram.h"

void SceneAnimatedEffect_LoadResources(u32);

s32 SceneTrailEffect_InitPool(u32 a)
{
    SceneAnimatedEffect_LoadResources(a);
    gSpriteNoticeManager = a;
    return 0;
}
