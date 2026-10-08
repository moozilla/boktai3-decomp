#include "global.h"

struct A { u8 f[0x34]; u32 flags; };
struct B { u8 f[0xc8]; u8 v; };

void sub_080515C0(struct A *a, s32 unused, struct B *b)
{
    u32 m = 0x200;
    if (a->flags & m)
        b->v = 1;
}
