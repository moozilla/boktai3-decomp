#include "global.h"

void SpriteGroup_RemoveSlot(u8 *, u8 *, s32);
void SpatialIndex_RemoveEntry(u8 *);

u32 SingletonSlot7_DestroyEffectPool(u8 *p)
{
    u8 *q = p + 0x20;
    s32 i = 0;
    do {
        if (*(u32 *)(p + 0x18) & (1 << i)) {
            SpriteGroup_RemoveSlot(p, q, i);
        }
        SpatialIndex_RemoveEntry(q + 0xC8);
        i++;
        q += 0x258;
    } while (i <= 3);
    return 0;
}
