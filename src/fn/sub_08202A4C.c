#include "global.h"
struct E { u8 p[0x128]; };
struct A { u8 p0[0x18]; u32 w18; u8 p1[0x1c - 0x1c]; struct E e[16]; };
extern u32 gUnk_020005FC;
void sub_0820270C(struct A *, struct E *, s32);
u32 sub_08202A4C(struct A *a)
{
    struct E *e = a->e;
    s32 i = 0;
    do {
        if (a->w18 & (1 << i))
            sub_0820270C(a, e, i);
        i++;
        e++;
    } while (i <= 0xf);
    return gUnk_020005FC = 0;
}
