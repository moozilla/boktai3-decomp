#include "global.h"

struct A { u8 f[0x34]; u32 a; u32 b; };
struct B { u8 f[0x43]; u8 v; };
struct C { u8 f0; u8 f1; u8 f2; u8 g[4]; u8 f7; u8 g2[2]; u16 fa; u8 g3[0xe]; u16 f1a; };

void sub_08009E5C(struct A *a, struct B *b, struct C *c)
{
    u32 m = 0x4000;
    if ((a->b & m) == 0) {
        u32 m2 = 0xe00;
        u32 x = a->a & m2;
        if (x == 0) {
            b->v = 1;
            c->f2 = 1;
            c->fa = x;
            c->f7 = 1;
            c->f1a |= 4;
        }
    }
}
