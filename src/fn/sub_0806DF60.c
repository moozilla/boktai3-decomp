#include "global.h"

void sub_08219DD8(void *, u32);

struct E { u8 a[0x153]; u8 idx; u8 b[0x178 - 0x153 - sizeof(u8)]; };
struct S { u8 pad0[0x1C]; u32 mask; struct E e[4]; };

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E *sub_0806DF60(struct S *b)
{
    s32 i;
    struct E *e;
    for (i = 0; i < 4; i++)
    {
        if (!Tst(&b->mask, i))
        {
            e = &b->e[i];
            b->mask |= 1 << i;
            sub_08219DD8(e, 0x178);
            e->idx = i;
            return e;
        }
    }
    return 0;
}
