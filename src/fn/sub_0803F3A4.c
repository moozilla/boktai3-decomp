#include "global.h"

void sub_0803EF4C(u8 *, u8 *, s32);

u32 sub_0803F3A4(u8 *p)
{
    u8 *e = p + 0x20;
    s32 i = 0;
    do {
        if (*(u32 *)(p + 0x18) & (1 << i))
            sub_0803EF4C(p, e, i);
        i++;
        e += 0x130;
    } while (i <= 7);
    return 0;
}
