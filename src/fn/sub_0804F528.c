#include "global.h"

struct D { u32 a; u32 b; };
struct S {
    u8 filler[0x18]; u32 flags; u8 f1c[0x18];
    struct D src;
    u8 f3c[0x254 - 0x3c];
    struct D dst;
};

void sub_0804F528(struct S *p)
{
    p->dst = p->src;
    if (p->flags & 4)
        *(u16 *)&p->dst.a += 0x100;
    else
        *(u16 *)&p->dst.b += 0x100;
}
