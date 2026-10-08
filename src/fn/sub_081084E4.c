#include "global.h"

void sub_08219DD8(void *, u32);

struct E { u8 a[0x62]; u16 idx; u8 b[0x68 - 0x62 - sizeof(u16)]; };
struct S { u8 pad0[0x1C]; struct E e[3]; u8 pad[0x154 - 0x1C - 0x68 * 3]; u32 mask; };

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E *sub_081084E4(struct S *b)
{
    s32 i;
    struct E *e;
    for (i = 0; i < 3; i++)
    {
        if (!Tst(&b->mask, i))
        {
            e = &b->e[i];
            b->mask |= 1 << i;
            sub_08219DD8(e, 0x68);
            e->idx = i;
            return e;
        }
    }
    return 0;
}
