#include "global.h"
struct A { u32 a, b, c; };
struct B { u32 flags : 16; u32 pad : 16; struct A *args; };
struct S { u8 f[2]; u8 t; u8 g[0xd]; u16 x; u16 y; u8 h[8]; u32 z; };
void sub_0821AD08(u32, struct B *);
void sub_08008BD0(struct S *p)
{
    struct A a;
    struct B b;
    if (p->y != 0) {
        a.a = p->x;
        if (p->t == 3)
            a.b = 1;
        else if (p->t == 4)
            a.b = 0;
        a.c = p->z;
        b.flags = 3;
        b.args = &a;
        sub_0821AD08(p->y, &b);
    }
}
