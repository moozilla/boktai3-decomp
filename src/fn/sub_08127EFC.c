#include "global.h"

void sub_08219DD8(void *, u32);

struct E { u8 a[0x9B]; u8 idx; u8 b[0xA0 - 0x9B - sizeof(u8)]; };
struct S { u8 pad0[0x38]; u32 mask; struct E e[12]; };

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E *sub_08127EFC(struct S *b)
{
    s32 i;
    struct E *e;
    for (i = 0; i < 12; i++)
    {
        if (!Tst(&b->mask, i))
        {
            e = &b->e[i];
            b->mask |= 1 << i;
            sub_08219DD8(e, 0xA0);
            e->idx = i;
            return e;
        }
    }
    return 0;
}
