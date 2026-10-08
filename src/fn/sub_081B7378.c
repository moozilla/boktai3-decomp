#include "global.h"

struct A { u32 a; u32 b; u8 f[0x18]; u32 c; u32 d; };
struct B { u32 a; u32 b; };

void sub_081B7378(struct A *p, struct B *q)
{
    u32 x, y;
    p->a = 8;
    p->b &= ~0x601;
    y = q->b;
    x = q->a;
    p->c = x;
    p->d = y;
}
