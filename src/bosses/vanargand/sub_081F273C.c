#include "global.h"

void SpriteTrail_InitializeEntry(u8 *, u8 *, s32);

u32 SingletonSlot7_InitEffectPool(u8 *p)
{
    u8 *q = p + 0x20;
    s32 i = 0;
    do {
        SpriteTrail_InitializeEntry(p, q, i);
        i++;
        q += 0x258;
    } while (i <= 3);
    return 0;
}
