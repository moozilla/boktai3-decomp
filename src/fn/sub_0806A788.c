#include "global.h"

void sub_08219DD8(void *, u32);

struct E { u8 a[0x73]; u8 idx; u8 b[0x7C - 0x74]; };
struct S { u8 pad0[0x18]; u32 mask; struct E e[24]; };

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E *sub_0806A788(struct S *b)
{
    s32 i;
    struct E *e;
    for (i = 0; i < 24; i++)
    {
        if (!Tst(&b->mask, i))
        {
            e = &b->e[i];
            b->mask |= 1 << i;
            sub_08219DD8(e, 0x7C);
            e->idx = i;
            return e;
        }
    }
    return 0;
}
