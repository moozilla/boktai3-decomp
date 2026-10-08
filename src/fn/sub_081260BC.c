#include "global.h"
extern u8 *gUnk_02000150;
u32 sub_08215184(u32);
u32 sub_081260BC(u8 *p)
{
    *(u32 *)(p + 0x18) = sub_08215184(0x1C1E);
    *(u32 *)(p + 0x69C) = 0;
    gUnk_02000150 = p;
    return 0;
}
