#include "global.h"

void sub_0803EF00(u8 *, u8 *, s32);
void sub_0821A2AC(u32);

u32 sub_0803F3D8(u8 *p)
{
    u8 *e = p + 0x20;
    s32 i = 0;
    do {
        sub_0803EF00(p, e, i);
        i++;
        e += 0x130;
    } while (i <= 7);
    sub_0821A2AC(0xFE2C);
    return 0;
}
