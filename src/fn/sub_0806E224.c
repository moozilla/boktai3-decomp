#include "global.h"
u32 sub_0821A520(u32, u32);
extern u8 *gUnk_02000198;
u32 sub_0806E224(u8 *p)
{
    *(u32 *)(p + 0x18) = sub_0821A520(0x922E, 0x1DC3);
    *(u32 *)(p + 0x1c) = 0;
    gUnk_02000198 = p;
    return 0;
}
