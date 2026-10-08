#include "global.h"

void sub_0801689C(u8 *p) {
    u32 z = 0;
    u32 one;
    *(u32 *)p = z;
    one = 1;
    p[4] = one;
    p[5] = z;
    p[6] = z;
    *(u32 *)(p + 0xC) = z;
    *(u32 *)(p + 0x2C) = (*(u32 *)(p + 0x2C) | one) & ~2;
}
