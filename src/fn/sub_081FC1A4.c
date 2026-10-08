#include "global.h"
struct E { u8 p[0x78]; };
extern u8 *gUnk_020005E4;
void sub_081FBEB8(u8 *, struct E *, s32);
void sub_082151E4(u8 *, u32);
u32 sub_0821A520(u32, u32);
u32 sub_081FC1A4(u8 *a)
{
    struct E *e;
    s32 i;
    gUnk_020005E4 = a;
    *(u32 *)(a + 0x18) = 0;
    e = (struct E *)(a + 0x38);
    i = 0;
    do {
        sub_081FBEB8(a, e, i);
        i++;
        e++;
    } while (i <= 0x1f);
    sub_082151E4(a + 0x1c, 0x8E5A);
    *(u32 *)(a + 0xF38) = sub_0821A520(0x922E, 0x4363);
    return 0;
}
