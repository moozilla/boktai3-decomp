#include "global.h"
extern u16 gUnk_03004BD8;
u32 sub_0821A520(u32, u32);
void sub_081BD300(u8 *p)
{
    u16 *r = &gUnk_03004BD8;
    u32 m = ~0x100;
    *r = m & *r;
    *(u32 *)(p + 0x8e0) = sub_0821A520(0xC091, 0x8544);
}
