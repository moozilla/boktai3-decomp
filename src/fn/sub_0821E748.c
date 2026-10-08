#include "global.h"
struct H { u16 n; };
struct A { u8 p[0x10]; struct H *f10; };
extern struct A *gUnk_030052F4;
u8 *sub_0821E748(u32 i)
{
    struct H *h = gUnk_030052F4->f10;
    return i < h->n ? (u8 *)h + 4 + (i << 2) : 0;
}
