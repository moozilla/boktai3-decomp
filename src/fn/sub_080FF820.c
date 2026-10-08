#include "global.h"
extern u8 *gUnk_020001A4;

static inline u8 Tst(u32 m, s32 one, s32 i)
{
    if (m & (one << i))
        return TRUE;
    return FALSE;
}

u8 *sub_080FF820(u32 x)
{
    u8 *b = gUnk_020001A4;
    s32 i;
    u32 m;
    s32 one;
    u8 *p;
    u8 *e;
    if (!b)
        return 0;
    i = 0;
    m = *(u32 *)(b + 0xA3C);
    one = 1;
    p = b + 0xD0;
    e = b + 0x1C;
    for (; i < 12; i++)
    {
        if (Tst(m, one, i))
        {
            if (*(u32 *)p == x)
                return e;
        }
        p += 0xD8;
        e += 0xD8;
    }
    return 0;
}
