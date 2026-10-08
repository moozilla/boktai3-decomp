#include "global.h"
u8 *sub_081FEA3C(u8 *p, s32 i)
{
    u8 *r;
    if (p != 0)
        r = (u8 *)(i * 0x3ac + (u32)p) + 0x88;
    else
        r = 0;
    return r;
}
