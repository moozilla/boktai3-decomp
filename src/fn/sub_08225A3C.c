#include "global.h"
struct S { u8 pad[0x18]; u32 f18; u32 f1c; };
extern struct S *gUnk_030025F8;
u32 sub_08225A3C(struct S *p)
{
    gUnk_030025F8 = p;
    p->f18 = 0;
    p->f1c = 0;
    return 0;
}
