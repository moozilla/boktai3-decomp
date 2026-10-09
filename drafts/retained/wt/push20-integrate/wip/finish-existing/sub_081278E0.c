#include "global.h"

extern u32 gUnk_02000164;
void sub_08214514(void *);

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

u32 sub_081278E0(u8 *p)
{
    s32 i = 0;
    u32 *m = (u32 *)(p + 0x860);
    for (; i < 10; i++)
    {
        if (Tst(m, i))
        {
            u32 o = i * 0xD4 + 0x18;
            u8 *e = p + o;
            sub_08214514(e);
        }
    }
    return gUnk_02000164 = 0;
}
