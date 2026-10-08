#include "global.h"

struct S { u8 f[0x33]; u8 a; };
extern struct S *gUnk_02000484;
extern u32 gUnk_020000F0;
u32 sub_08036BEC(void)
{
    struct S *p = gUnk_02000484;
    u32 r;
    if (p != 0) r = p->a;
    else r = gUnk_020000F0;
    return r;
}
