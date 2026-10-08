#include "global.h"

extern u8 *gUnk_020000DC;
u32 sub_08032CD4(u8 *, u32, u32);
u32 sub_08031018(u32 a, u32 b)
{
    u8 *p = gUnk_020000DC;
    u32 r;
    if (p == 0) r = 0;
    else r = sub_08032CD4(p + 0x18, a, b);
    return r;
}
