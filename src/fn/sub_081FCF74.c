#include "global.h"
struct E { u8 p[0x1c0]; };
struct A { u8 p0[0x18]; u32 w18; u8 p1[0x20 - 0x1c]; struct E e[10]; };
extern struct A *gUnk_020005E8;
void sub_081FC900(struct A *, struct E *, s32);
u32 sub_081FCF74(struct A *a)
{
    struct E *e;
    s32 i;
    gUnk_020005E8 = a;
    a->w18 = 0;
    e = a->e;
    i = 0;
    do {
        sub_081FC900(a, e, i);
        i++;
        e++;
    } while (i <= 9);
    return 0;
}
