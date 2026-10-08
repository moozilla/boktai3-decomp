#include "global.h"

s32 sub_0821ABA8(u32, u32);
s32 sub_08003BB8(s32, s32);
s32 sub_08004194(void)
{
    s32 a = sub_0821ABA8(0x6d, 0);
    s32 b = sub_0821ABA8(0x6e, 0);
    if (b == 0)
        return -1;
    return sub_08003BB8(a, b);
}
