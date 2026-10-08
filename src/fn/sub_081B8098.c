#include "global.h"

struct A { u8 f[0x18]; u8 b; u8 g[0x4a3]; u32 w; };
u32 sub_0802140C(s32);
s32 sub_081A32E0(u32, void *);

s32 sub_081B8098(struct A *p)
{
    p->b = 0;
    p->w = sub_0802140C(1);
    return sub_081A32E0(p->w, p);
}
