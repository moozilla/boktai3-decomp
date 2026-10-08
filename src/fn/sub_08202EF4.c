#include "global.h"
struct S { u8 p[0x14]; u32 w14; u32 w18; u16 h1c; u16 h1e; u16 pad; u16 h22; };
void sub_08202EF4(struct S *p, u32 a, u32 b, u32 c, u32 d, u32 e)
{
    p->w14 = a;
    p->w18 = b;
    p->h1c = c;
    p->h1e = d;
    p->h22 = e;
}
