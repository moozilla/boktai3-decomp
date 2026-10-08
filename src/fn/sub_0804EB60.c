#include "global.h"

struct S { u8 filler[0x1c]; u16 v; u8 f1e[6]; u16 a; u8 b; u8 c; u8 d; };
extern struct S *gUnk_0200010C;
u16 sub_0804EB38(struct S *);

s32 sub_0804EB60(struct S *p)
{
    u8 z;
    p->v = sub_0804EB38(p);
    z = 0;
    p->b = z;
    p->a = 0x1C7;
    p->d = z;
    gUnk_0200010C = p;
    return 0;
}
