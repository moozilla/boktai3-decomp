#include "global.h"
u8 *sub_0821A2C4(u32);
u8 *sub_081DAE64(s32 i)
{
    u8 *p = sub_0821A2C4(0x4E01);
    if (p == 0 || i < 0 || i >= *(s32 *)(p + 0x3c)) return 0;
    return *(u8 **)(p + 0x108) + i * 0x1ac + 0xa8;
}
