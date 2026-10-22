#include "global.h"

bool32 sub_081FEA28(u8 *p, s32 bit)
{
    u32 r = *(u32 *)(p + 0x18) & (1 << bit);
    if (r)
        r = TRUE;
    return r;
}
