#include "global.h"

u8 *sub_08179300(u32 i, u32 x, u32 y)
{
    u8 *tbl = (u8 *)0x03004C30 + ((i * 3) << 4);
    u8 *base = *(u8 **)(tbl + 0x2C);

    return base + ((x & 0x1F) << 1) + ((y & 0x1F) << 6);
}
