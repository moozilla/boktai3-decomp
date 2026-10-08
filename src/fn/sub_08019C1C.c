#include "global.h"
extern u32 gUnk_020000B0;
u32 sub_0821A520(u32, u32);
void sub_08220C8C(u8 *, u32, u32, u32, u32);
void sub_08019C1C(u8 *p, u32 a, u32 b, u32 c, u32 d)
{
    if (gUnk_020000B0 != 0) {
        u32 r = sub_0821A520(0x922e, (u16)a);
        *(u32 *)(p + 0x64) = r;
        sub_08220C8C(p + 0x54, r, (u16)b, (u8)c, (u8)d);
    }
}
