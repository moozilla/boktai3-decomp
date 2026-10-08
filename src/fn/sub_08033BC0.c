#include "global.h"
struct S { u8 pad[0x18]; u32 f18; u32 f1c; };
extern struct S *gUnk_020000E4;
u32 sub_08033BC0(struct S *p)
{
    gUnk_020000E4 = p;
    p->f18 = 0;
    p->f1c = 0;
    return 0;
}
