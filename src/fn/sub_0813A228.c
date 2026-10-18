#include "global.h"
void SpritePair_ReleaseSlot(u32, u32);
extern u32 gUnk_020004D4;
u32 FollowingSpriteEffect_DestroyManager(void)
{
    s32 i = 0;
    do {
        SpritePair_ReleaseSlot(i, 1);
        i++;
    } while (i <= 1);
    return gUnk_020004D4 = 0;
}
