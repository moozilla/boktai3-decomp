#include "global.h"
#include "ram.h"
void AnimData_Load(void *, u32);
void Sprite_InitUnlinked(void *, void *, u32);
u32 TransientSpritePool_Init(u8 *s)
{
    u8 *p;
    s32 i;
    gTransientSpritePool = s;
    p = s + 0x18;
    i = 0xf;
    do {
        u8 *q = p + 0x30;
        AnimData_Load(q, 0xda6d);
        Sprite_InitUnlinked(p + 4, q, 0);
        i--;
        p += 0x4c;
    } while (i >= 0);
    return 0;
}
