#include "global.h"

extern u32 gUnk_020004C8;
void SlotRegistry_RemoveSprite(s32);

s32 NativeEffect_DestroyManager(void)
{
    s32 i;
    for (i = 0; i <= 0xf; i++)
        SlotRegistry_RemoveSprite(i);
    return gUnk_020004C8 = 0;
}
