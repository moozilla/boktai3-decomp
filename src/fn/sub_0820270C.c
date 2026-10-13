#include "global.h"
struct A {
    u8 p0[0x18];
    u32 w18;
};
void Sprite_Remove(void *);
void sub_0820270C(struct A *a, u32 *b, u32 n)
{
    u32 m, t;
    u32 *q = (u32 *)((u8 *)b + 0xA0);
    t = *q;
    m = 1;
    *q = t | m;
    Sprite_Remove(q);
    a->w18 &= ~(m << n);
}
