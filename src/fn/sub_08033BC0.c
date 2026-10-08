#include "global.h"

struct S { u8 f[0x18]; u32 a; u32 b; };
extern struct S *gUnk_020000E4;
u32 sub_08033BC0(struct S *p)
{
    gUnk_020000E4 = p;
    p->a = 0;
    p->b = 0;
    return 0;
}
