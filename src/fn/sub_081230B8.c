#include "global.h"

extern u32 gUnk_02000144;
void sub_08217EAC(void *);

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E { u8 a[0x48]; };
struct S { u8 pad[0x20]; struct E e[32]; u8 pad2[0x920 - 0x20 - 0x48 * 32]; u32 mask; };

u32 sub_081230B8(struct S *b)
{
    s32 i = 0;
    u32 *m = &b->mask;
    struct E *e = b->e;
    for (; i < 32; e++, i++)
    {
        if (Tst(m, i))
            sub_08217EAC(e);
    }
    {
        u32 z = 0;
        gUnk_02000144 = z;
    }
    return 0;
}
