#include "global.h"

struct E { u8 a[0x123]; u8 idx; u8 b[0x128 - 0x124]; };
struct S { u8 pad0[0x38]; u32 mask; struct E e[8]; };

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E *sub_0812B7AC(struct S *b)
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
