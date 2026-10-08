#include "global.h"

struct E { u8 a[0x1A]; u16 idx; u8 b[4]; };
struct S { u8 pad0[0x18]; struct E e[12]; u8 pad[0x198 - 0x18 - 0x20 * 12]; u32 mask; };

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E *sub_0806F400(struct S *b)
{
    s32 i;
    for (i = 0; i < 12; i++)
    {
        if (!Tst(&b->mask, i))
        {
            b->mask |= 1 << i;
            b->e[i].idx = i;
            return &b->e[i];
        }
    }
    return 0;
}
