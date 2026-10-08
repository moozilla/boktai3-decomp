#include "global.h"

void sub_0822B2F8(u32);

static inline void Clr(u16 *q, u32 m)
{
    *q = m & *q;
}

static inline u8 Tst(u16 *m, u32 b)
{
    if (*m & b)
        return TRUE;
    return FALSE;
}

void sub_08090A98(u8 *p)
{
    u8 *q = *(u8 **)(p + 0x3D0);
    u32 m = 0x10;
    if (Tst((u16 *)(q + 0x40A), m))
    {
        sub_0822B2F8(0x3C2);
        Clr((u16 *)(q + 0x40A), ~0x10);
    }
}
