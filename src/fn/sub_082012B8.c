#include "global.h"
struct S { u8 p[0x10]; u16 h10; u16 h12; u16 h14; u16 pad; u16 h18; u16 h1a; u16 h1c; };
void sub_082012B8(struct S *p, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f)
{
    p->h10 = a;
    p->h12 = b;
    p->h14 = c;
    p->h18 = d;
    p->h1a = e;
    p->h1c = f;
}
