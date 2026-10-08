#include "global.h"
struct E { u8 p[0x160]; };
struct A { u8 p0[0x18]; u32 w18; u8 p1[0x1c - 0x1c]; struct E e[3]; };
extern u32 gUnk_020005F4;
void sub_0821FE6C(void *);
void sub_081FFA74(struct A *, struct E *, s32);
u32 sub_081FFD78(struct A *a)
{
    struct E *e = a->e;
    s32 i = 0;
    do {
        if (a->w18 & (1 << i)) {
            sub_0821FE6C((u8 *)e + 0x78);
            sub_081FFA74(a, e, i);
        }
        i++;
        e++;
    } while (i <= 2);
    return gUnk_020005F4 = 0;
}
