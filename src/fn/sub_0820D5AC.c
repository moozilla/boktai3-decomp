#include "global.h"
struct E { u8 p[0xB0]; };
struct A { u8 p0[0x18]; u32 w18; u8 p1[0x1C - 0x1c]; struct E e[24]; };
extern u32 gUnk_02000604;
void sub_0820D2A0(struct A *, struct E *, s32);
u32 sub_0820D5AC(struct A *a)
{
    struct E *e = a->e;
    s32 i = 0;
    do {
        if (a->w18 & (1 << i))
            sub_0820D2A0(a, e, i);
        i++;
        e++;
    } while (i <= 0xF);
    return gUnk_02000604 = 0;
}
