#include "global.h"

struct A {
    u8 f[0x9c];
    u32 fl;
    u8 g[0xe9e];
    u16 h;
};

void Vanargand_SetPendingCue(struct A *p, u16 v)
{
    u32 m = 0x200000;
    p->fl |= m;
    p->h = v;
}
