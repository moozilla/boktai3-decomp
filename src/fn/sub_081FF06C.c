#include "global.h"
struct E { u8 p[0xB8]; };
struct A { u8 p0[0x18]; u32 w18; u8 p1[0x1C - 0x1c]; struct E e[24]; };
struct E *sub_081FF06C(struct A *a, s32 *out)
{
    struct E *e = a->e;
    s32 i = 0;
    u32 m = 1;
    u32 w = a->w18;
    do {
        if ((m << i & w) != 0) { i++; e++; continue; }
        *out = i; return e;
    } while (i <= 0x1F);
    return 0;
}
