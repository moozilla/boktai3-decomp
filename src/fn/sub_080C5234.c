#include "global.h"

struct P { u8 f[0x26c]; void *cb; u8 g[0x2d4 - 0x270]; u16 fl; };
u32 sub_0824923C(void *, void *);
u8 sub_080C5234(struct P *p)
{
    u32 m = 0x100;
    u8 r;
    if (p->fl & m)
        r = sub_0824923C(p, p->cb);
    else
        r = 0;
    return r;
}