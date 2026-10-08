#include "global.h"

struct A { u8 f[0x38]; u32 fl; u8 g[2]; u16 d; };
struct B { u8 f[2]; u8 t; u8 g[0xd]; s16 v; };

void sub_0800CBAC(struct A *a, u32 unused, struct B *b)
{
    struct B *c = b;
    if (c->t == 0) {
        u32 fl = a->fl;
        u32 d;
        d = a->d;
        if (fl & 8) {
            d <<= 1;
            b->v -= d;
        }
        if (c->v < 0)
            c->v = 0;
    }
}
