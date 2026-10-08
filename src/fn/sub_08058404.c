#include "global.h"
struct P { u8 f[0x22e]; u16 f22e; u8 f230[0x1428 - 0x230]; u32 f1428; };
void sub_08058130(struct P *, u32, u32);
void sub_08058150(struct P *, u32);
void sub_08058404(struct P *p)
{
    u16 *c = &p->f22e;
    u32 v = *c + 1;
    *c = v;
    if ((u16)v > 5) {
        sub_08058130(p, 2, 2);
        sub_08058150(p, 0);
        p->f1428 = 1;
    }
}
