#include "global.h"

struct E { u8 a[0x3D]; u8 idx; u8 b[0x138 - 0x3E]; };
struct S { u8 pad0[0x5C]; struct E e[8]; u8 pad[0xA1C - 0x5C - 0x138 * 8]; u32 mask; };

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E *sub_08065BEC(struct S *b)
{
    s32 i;
    struct E *e;
    for (i = 0; i < 8; i++)
    {
        if (!Tst(&b->mask, i))
        {
            e = &b->e[i];
            b->mask |= 1 << i;
            e->idx = i;
            return e;
        }
    }
    return 0;
}
