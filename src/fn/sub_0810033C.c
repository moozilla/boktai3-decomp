#include "global.h"

struct E { u8 a[0x84]; u16 idx; u8 b[10]; };
struct S { u8 pad0[0x18]; struct E e[8]; u8 pad[0x498 - 0x18 - 0x90 * 8]; u32 mask; };

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E *sub_0810033C(struct S *b)
{
    s32 i;
    struct E *e;
    for (i = 0; i < 8; i++)
    {
        if (!Tst(&b->mask, i))
        {
            b->mask |= 1 << i;
            e = &b->e[i];
            e->idx = i;
            return e;
        }
    }
    return 0;
}
