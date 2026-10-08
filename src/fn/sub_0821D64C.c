#include "global.h"
struct N { u16 f0; u16 f2; u8 f4; u8 p5[7]; struct N *fc; };
struct A { u8 p[0x18]; struct N *f18; };
extern struct A *gUnk_030052F4;
struct N *sub_0821D64C(u32 k, u32 m)
{
    struct N *best;
    struct N *n = gUnk_030052F4->f18;
    best = 0;
    for (; n; n = n->fc) {
        if ((n->f0 & m) == 0 && n->f2 == k) {
            if (best == 0 || (best->f4 & 0xF) < (n->f4 & 0xF))
                best = n;
        }
    }
    return best;
}
