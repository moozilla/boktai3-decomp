#include "global.h"
extern u8 *gUnk_0200019C;

static inline u8 Tst(u32 m, s32 one, s32 i)
{
    if (m & (one << i))
        return TRUE;
    return FALSE;
}

u8 *sub_0806F3A8(u32 x)
{
    u8 *b = gUnk_0200019C;
    s32 i;
    u32 m;
    s32 one;
    u8 *p;
    if (!b)
        return 0;
    i = 0;
    m = *(u32 *)(b + 0x198);
    p = b;
    one = 1;
    for (; i < 12; i++)
    {
        if (Tst(m, one, i))
        {
            u32 o = i * 0x20;
            if (*(u16 *)(p + 0x2C) == x)
                return b + (o + 0x18);
        }
        p += 0x20;
    }
    return 0;
}
