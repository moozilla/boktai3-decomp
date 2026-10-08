#include "global.h"

struct P { u32 a; u32 b; };
struct O { u32 w0; struct P p4; struct P pc; u8 g[0x14]; u8 b28; u8 b29; u8 g2[2]; u16 h2c; u16 h2e; u16 h30; u16 h32; u32 w34; };

void sub_081B9818(struct O *o, struct P *d, struct P *a, struct P *b, u16 h)
{
    o->w0 = (u32)d;
    o->p4 = *a;
    o->pc = *b;
    *d = *a;
    o->b28 = 0xc;
    o->h32 = h;
    o->w34 = 0;
    o->b29 = 1;
}
