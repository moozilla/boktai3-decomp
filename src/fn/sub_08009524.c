#include "global.h"

struct Sub { u8 f[7]; u8 b; u8 g[8]; u16 v; };
struct S { u8 f[0x24]; struct Sub a; u8 h[0x50-0x24-sizeof(struct Sub)]; u8 c[1]; };
s32 sub_082151E4(void *, u32);
void sub_082144A4(void *, void *, u32);

s32 sub_08009524(u32 unused, struct S *s)
{
    void *p = s->c;
    s32 r;
    if (sub_082151E4(p, 0xA850) != 0) {
        struct Sub *q = &s->a;
        sub_082144A4(q, p, 1);
        q->v = 1;
        s->f[0x2b] = 2;
        r = 0;
    } else
        r = -1;
    return r;
}
