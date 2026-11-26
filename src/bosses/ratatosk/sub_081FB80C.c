#include "global.h"
s32 sub_081FB80C(u8 *p)
{
    u32 bit;
    *(s32 *)(p + 0x60) += *(s32 *)(p + 0x20);
    *(s32 *)(p + 0x5c) += *(s32 *)(p + 0x60);
    bit = 1;
    *(u32 *)(p + 0x58) |= bit;
    return 0;
}