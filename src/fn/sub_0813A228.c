#include "global.h"
#include "ram.h"
void SpritePair_ReleaseSlot(u32, u32);
u32 FollowingSpriteEffect_DestroyManager(void)
{
    s32 i = 0;
    do {
        SpritePair_ReleaseSlot(i, 1);
        i++;
    } while (i <= 1);
    return gSpritePairPool = 0;
}
