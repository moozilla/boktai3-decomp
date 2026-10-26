#include "global.h"
#include "ram.h"
void AnimData_Load(u8 *, u32);
void Sprite_InitUnlinked(u8 *, u8 *, u32);
u32 SpritePool_Initialize(u8 *p)
{
    u8 *e;
    u8 *q;
    s32 i;
    gSpritePoolManager = p;
    e = p + 0x18;
    i = 0xf;
    do {
        q = e + 0x38;
        AnimData_Load(q, 0xda6d);
        Sprite_InitUnlinked(e + 0xc, q, 0);
        i--;
        e += 0xb0;
    } while (i >= 0);
    return 0;
}
