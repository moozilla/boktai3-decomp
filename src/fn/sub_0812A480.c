#include "global.h"

extern u32 gUnk_0200017C;
void sub_08214514(void *);

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E { u8 a[0xFC]; };
struct S { u8 pad[0x38]; u32 mask; struct E e[12]; };

u32 sub_0812A480(struct S *b)
{
    s32 i = 0;
    struct E *e = b->e;
    for (; i < 12; e++, i++)
    {
        if (Tst(&b->mask, i))
            sub_08214514(e);
    }
    {
        u32 z = 0;
        gUnk_0200017C = z;
    }
    return 0;
}
