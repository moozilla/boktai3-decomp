#include "global.h"
struct P { u8 f[0x22e]; u16 f22e; };
void sub_08058130(struct P *, u32, u32);
void sub_08058150(struct P *, u32);
void sub_08058284(struct P *p)
{
    u16 *c = &p->f22e;
    u32 v = *c + 1;
    *c = v;
    if ((u16)v > 7) {
        sub_08058130(p, 2, 2);
        sub_08058150(p, 0);
    }
}
