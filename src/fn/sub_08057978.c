#include "global.h"
struct A { u32 a; };
struct B { u32 flags; struct A *args; };
void sub_0821AD08(u32, struct B *);
void sub_08057978(u8 *p, u32 x)
{
    u32 r = *(u32 *)(p + 0x17ac);
    if (r != 0) {
        struct A a;
        struct B b;
        b.flags = (b.flags & 0xFFFF0000) | 1;
        a.a = x;
        b.args = &a;
        sub_0821AD08(r, &b);
    }
}
