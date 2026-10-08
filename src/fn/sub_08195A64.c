#include "global.h"
struct P { u8 f[0x5404]; u16 a, b, c; u8 g[0x724A-0x540A]; u8 s; };
void sub_08193680(struct P *, s32);
void sub_08195A64(struct P *p)
{
    u8 *q = &p->s;
    u32 z = *q;
    if (z == 0) {
        sub_08193680(p, 0);
        p->a = 0x260;
        p->b = z;
        p->c = 0x500;
        (*q)++;
    }
}
