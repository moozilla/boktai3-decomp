#include "global.h"
struct O { u8 f[8]; u32 fl; u8 g[0x14]; u16 h; };
void sub_0801C154(u8 *p)
{
    struct O *o = (struct O *)(p + 0x1a4);
    o->h = p[0xc0];
    o->fl &= ~1;
}
