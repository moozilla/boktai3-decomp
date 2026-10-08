#include "global.h"

void sub_08249240(u32, u32, u32);

void sub_08224980(u8 a, u8 b) {
    u8 *p = (u8 *)0x03005390;
    u32 f = *(u32 *)(p + 0x40);
    if (f != 0)
        sub_08249240(a, b, f);
    *(u16 *)(p + 0x16) = 0;
    *(u16 *)(p + 0x14) = 0;
}
