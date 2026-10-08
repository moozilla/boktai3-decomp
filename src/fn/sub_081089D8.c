#include "global.h"

extern u32 gUnk_020001BC;
void sub_08214514(void *);

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

u32 sub_081089D8(u8 *p)
{
    s32 i = 0;
    u32 *m = (u32 *)(p + 0x154);
    for (; i < 3; i++)
    {
        if (Tst(m, i))
        {
            u32 o = i * 0x68 + 0x1C;
            u8 *e = p + o;
            sub_08214514(e);
        }
    }
    return gUnk_020001BC = 0;
}
