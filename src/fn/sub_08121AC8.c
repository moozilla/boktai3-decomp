#include "global.h"

extern u32 gUnk_0200012C;
void sub_08214514(void *);

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

u32 sub_08121AC8(u8 *p)
{
    s32 i = 0;
    
    for (; i < 4; i++)
    {
        if (Tst((u32 *)(p + 0x38), i))
        {
            u32 o = i * 0xA4 + 0x3C;
            u8 *e = p + o;
            sub_08214514(e);
        }
    }
    return gUnk_0200012C = 0;
}
