#include "global.h"

struct Q { u8 filler[0xe4c]; u32 f; };
struct B { u8 pad[0xf]; u8 b; };
struct S080FAE08 {
    u8 filler0[0xdc]; struct B bb;
    u8 filler1[0x2d4 - 0xec]; u16 h;
    u8 filler2[0x3d0 - 0x2d6]; struct Q *q;
    u8 filler3[0x418 - 0x3d4]; u16 z;
};

void sub_080FAE08(struct S080FAE08 *s)
{
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
    s->z = zero;
}
