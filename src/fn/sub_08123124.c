#include "global.h"

extern u32 gUnk_02000144;
void SceneAnimatedEffect_LoadResources(u32);

s32 SceneTrailEffect_InitPool(u32 a)
{
    SceneAnimatedEffect_LoadResources(a);
    gUnk_02000144 = a;
    return 0;
}
