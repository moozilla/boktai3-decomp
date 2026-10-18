#include "global.h"

extern u32 gUnk_020004D8;
void SpritePair_RemoveSlot(s32);

s32 PositionLinkedSprite_DestroyManager(void)
{
    s32 i;
    for (i = 0; i <= 1; i++)
        SpritePair_RemoveSlot(i);
    return gUnk_020004D8 = 0;
}
