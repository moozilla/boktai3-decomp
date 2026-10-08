#include "global.h"
struct S { u8 f[0x18]; u32 a; u32 b; };
extern struct S *gUnk_02000598;
s32 sub_0815F77C(struct S *s)
{
    s->a = 0;
    s->b = 0;
    gUnk_02000598 = s;
    return 0;
}
