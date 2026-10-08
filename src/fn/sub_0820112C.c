#include "global.h"
struct E { u8 p[0x344]; };
struct A { u8 p0[0x18]; u32 w18; u8 p1[0x1c - 0x1c]; struct E e[3]; };
struct E *sub_0820112C(struct A *a, s32 *out)
{
    struct E *e = a->e;
    s32 i = 0;
    u32 m = 1;
    u32 w = a->w18;
    do {
        if ((m << i & w) != 0) { i++; e++; continue; }
        *out = i; return e;
    } while (i <= 2);
    return 0;
}
