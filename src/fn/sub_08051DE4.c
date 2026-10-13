#include "global.h"
extern u8 *gUnk_02000114;
void Sprite_Remove(u8 *);
void Effect_Remove(u8 *);
u32 sub_08051DE4(u8 *p)
{
    u8 *q;
    s32 i;
    Sprite_Remove(p + 0x18);
    Sprite_Remove(p + 0x60);
    q = p + 0xC0;
    i = 7;
    do {
        Effect_Remove(q);
        q += 0x34;
        i--;
    } while (i >= 0);
    return 0;
}
