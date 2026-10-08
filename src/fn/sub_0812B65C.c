#include "global.h"
struct W { u8 f[6]; u16 v; };
void sub_0812B65C(u32 a, u32 b, u8 *p)
{
    u8 *q = p;
    struct W *w;
    p[0x1B6] = 2;
    p[0x1BA] = 1;
    w = (struct W *)(q + 0xf4);
    w->v |= 4;
    w = (struct W *)(q + 0x148);
    w->v |= 4;
}
