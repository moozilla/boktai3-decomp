#include "global.h"

struct A { u8 f[0x9c]; u32 fl; u8 g[0xe9e]; u16 h; };

void sub_081A6E9C(struct A *p, u16 v)
{
    u32 m = 0x200000;
    p->fl |= m;
    p->h = v;
}
