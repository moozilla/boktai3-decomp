#include "global.h"

struct Q {
    u8 filler[0x558];
    u32 f;
    u8 filler2[0x624 - 0x55C];
    u16 z;
};
struct B {
    u8 pad[0xf];
    u8 b;
};
struct S {
    u8 filler0[0xdc];
    struct B bb;
    u8 filler1[0x2d4 - 0xec];
    u16 h;
    u8 filler2[0x3d0 - 0x2d6];
    struct Q *q;
};

void Bee_ClearActionFlags(struct S *s)
{
    struct Q *q = s->q;
    u16 *hp = &s->h;
    u32 hm = 0xFFFFFEFF;
    u32 *p;
    u32 m;
    struct B *t;
    u32 bm;
    u16 zero;
    u16 hv;
    hv = *hp & hm;
    zero = 0;
    *hp = hv;
    t = &s->bb;
    bm = ~8;
    t->b = t->b & bm;
    p = &s->q->f;
    m = ~0x10;
    *p = *p & m;
    q->z = zero;
}
