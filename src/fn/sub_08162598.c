#include "global.h"

extern u32 gUnk_0200059C;
s32 Sprite_Remove(u8 *);

s32 sub_08162598(u8 *p)
{
    u8 *q = p + 0xc4;
    s32 i = 15;
    u32 z;
    do {
        Sprite_Remove(q);
        q += 0x2c;
        i--;
    } while (i >= 0);
    z = 0;
    gUnk_0200059C = z;
}
