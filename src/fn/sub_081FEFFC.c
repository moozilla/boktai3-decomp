#include "global.h"
struct E { u8 p[0xb8]; };
struct A { u8 p0[0x18]; u32 w18; u8 p1[0x1c - 0x1c]; struct E e[32]; };
extern struct A *gUnk_020005F0;
void sub_081FEDA0(struct A *, struct E *, s32);
u32 sub_081FEFFC(struct A *a)
{
    struct E *e;
    s32 i;
    gUnk_020005F0 = a;
    a->w18 = 0;
    e = a->e;
    i = 0;
    do {
        sub_081FEDA0(a, e, i);
        i++;
        e++;
    } while (i <= 0x1f);
    return 0;
}
