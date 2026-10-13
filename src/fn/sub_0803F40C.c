#include "global.h"

void sub_0821A284(u32, u8 *, u32);
u32 sub_0821A520(u32, u32);
void sub_0803EE5C(u8 *, u8 *, s32);

u32 sub_0803F40C(u8 *p)
{
    u8 *e;
    s32 i;
    sub_0821A284(0xFE2C, p, 0);
    *(u32 *)(p + 0x18) = 0;
    *(u32 *)(p + 0x1c) = sub_0821A520(0x922E, 0x871C);
    e = p + 0x20;
    i = 0;
    do {
        sub_0803EE5C(p, e, i);
        i++;
        e += 0x130;
    } while (i <= 7);
    return 0;
}
