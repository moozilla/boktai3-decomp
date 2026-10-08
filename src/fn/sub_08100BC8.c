#include "global.h"

struct E { u8 a[0xB4]; u16 idx; u8 b[0x19C - 0xB6]; };
struct S { u8 pad0[0x1C]; struct E e[12]; u8 pad[0x136C - 0x1C - 0x19C * 12]; u32 mask; };

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E *sub_08100BC8(struct S *b)
{
    s32 i;
    struct E *e;
    for (i = 0; i < 12; i++)
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
