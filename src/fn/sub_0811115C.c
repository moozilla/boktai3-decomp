#include "global.h"
struct S { u8 f[0x44]; s32 a; u8 g[4]; s32 b; };
extern struct S *gUnk_020004B8;
s32 sub_0811115C(void)
{
    struct S *s = gUnk_020004B8;
    if (!s) return 0;
    else return s->a - s->b;
}
