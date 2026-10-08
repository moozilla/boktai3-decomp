#include "global.h"
extern const s16 gUnk_086149C4[];
struct P { u32 a, b; };
void sub_0804AD5C(u8 *p)
{
    u32 i;
    s32 x;
    s32 y;
    s32 t;
    *(struct P *)(p + 0x474) = *(struct P *)(p + 0x50);
    i = ((p[0x260] + 1) & 7) << 5;
    t = gUnk_086149C4[(i + 0x40) & 0xFF] << 7;
    if (t >= 0) x = t >> 12;
    else x = -(-t >> 12);
    *(u16 *)(p + 0x474) += x;
    t = gUnk_086149C4[i] << 7;
    if (t >= 0) y = t >> 12;
    else y = -(-t >> 12);
    *(u16 *)(p + 0x478) += y;
}
