#include "global.h"

u32 sub_08042320(u8 *, u32, u32, u32, u32, u32, u32);
u32 sub_0802DCB8(u8 *p, u32 x, u32 y, u32 z, u32 s0, u32 s1)
{
    return sub_08042320(p + 0xd4, x, 0, 0, y, s0, s1);
}
