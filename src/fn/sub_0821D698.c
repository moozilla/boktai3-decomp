#include "global.h"
struct N { u16 f0; u16 f2; u8 f4; u8 f5; u16 f6; struct N *f8; struct N *fc; };
struct A { u8 p[0x18]; struct N *f18; };
extern struct A *gUnk_030052F4;
void sub_0821D630(struct N *, u32, u32, u32, u32, u32);
u32 sub_0821D698(struct N *n, u32 a, u32 b, u32 c, u32 d, u32 e)
{
    sub_0821D630(n, a, b, c, d, e);
    n->f8 = 0;
    n->fc = gUnk_030052F4->f18;
    if (n->fc) n->fc->f8 = n;
    gUnk_030052F4->f18 = n;
    return 0;
}
