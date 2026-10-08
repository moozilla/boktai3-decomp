#include "global.h"
extern u8 *gUnk_02000710;

static inline u8 Eq(u32 a, u32 b)
{
    if (a == b)
        return TRUE;
    return FALSE;
}

u32 sub_0810038C(u32 *p)
{
    u32 v = p[0x54 / 4];
    if (Eq(*(u32 *)(gUnk_02000710 + 0x5A0), v))
    {
        *p &= ~1;
        return 1;
    }
    *p |= 1;
    return 0;
}
