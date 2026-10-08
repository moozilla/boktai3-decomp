#include "global.h"
u8 sub_08138248(u8 *, u32);
s32 sub_08138288(u8 *p)
{
    s32 v = *(s32 *)(p + 0x48);
    if (v >= 0 && v == *(s16 *)(p + 0x18) && p[0x1f] <= 0xe) {
        if (sub_08138248(p, *(u16 *)(p + 0x1a)))
            goto zero;
    }
    return -1;
zero:
    return 0;
}
