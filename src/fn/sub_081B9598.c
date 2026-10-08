#include "global.h"

struct P { u32 a; u32 b; };
struct O { u32 w0; u8 g[8]; struct P pc; u8 g2[0x14]; u8 b28; u8 b29; u8 b2a; u8 b2b; u16 h2c; u16 h2e; u16 h30; u16 pad; u32 w34; };

void sub_081B9598(struct O *o, u32 d, struct P *a, u32 h1, u32 h2, u32 h3, u32 b1, u32 b2)
{
    if (o != 0 && d != 0 && a != 0) {
        o->w0 = d;
        o->pc = *a;
        o->h2c = h1;
        o->h2e = h2;
        o->h30 = h3;
        o->b2b = b2;
        o->b2a = b1;
        o->b28 = 10;
        o->b29 = 1;
        o->w34 = 0;
    }
}
