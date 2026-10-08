#include "global.h"
struct E { u8 p[0x128]; };
struct A { u8 p0[0x18]; u32 w18; u8 p1[0x1c - 0x1c]; struct E e[16]; };
extern struct A *gUnk_020005FC;
void sub_0820267C(struct A *, struct E *, s32);
u32 sub_08202A88(struct A *a)
{
    struct E *e;
    s32 i;
    gUnk_020005FC = a;
    a->w18 = 0;
    e = a->e;
    i = 0;
    do {
        sub_0820267C(a, e, i);
        i++;
        e++;
    } while (i <= 0xf);
    return 0;
}
