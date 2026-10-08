#include "global.h"
struct E { u8 p[0x1c0]; };
struct A { u8 p0[0x18]; u32 w18; u8 p1[0x20 - 0x1c]; struct E e[10]; };
extern u32 gUnk_020005E8;
void sub_081FCA0C(struct A *, struct E *, s32);
u32 sub_081FCF38(struct A *a)
{
    struct E *e = a->e;
    s32 i = 0;
    do {
        if (a->w18 & (1 << i))
            sub_081FCA0C(a, e, i);
        i++;
        e++;
    } while (i <= 9);
    return gUnk_020005E8 = 0;
}
