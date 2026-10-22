#include "global.h"

s32 sub_081C61AC(u8 *p)
{
    u32 *q = (u32 *)(p + 0xb8);
    q[2] &= ~1;
}
