#include "global.h"
struct A { u32 a, b; };
struct B { u32 flags : 16; u32 pad : 16; struct A *args; };
void sub_0821AD08(u32, struct B *);
void sub_0801A094(u8 *p)
{
    struct A a;
    struct B b;
    b.flags = 2;
    b.args = &a;
    a.a = p[0x21];
    a.b = p[0x22];
    sub_0821AD08(*(u16 *)(p + 0x42), &b);
}
