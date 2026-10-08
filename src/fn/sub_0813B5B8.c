#include "global.h"

extern u32 gUnk_030053F4;

s32 sub_0813B5B8(u8 *p)
{
    u8 *q = p + 0x32C;
    s32 s;
    if (gUnk_030053F4 & 0x1000)
        return 0xc;
    s = *(s16 *)(q + 8) + *(u16 *)(p + 0x41E);
    if (s > 0x78)
        s = 0x78;
    return s * 5 + 100;
}
