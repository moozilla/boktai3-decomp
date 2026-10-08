#include "global.h"

struct P { u8 f[0x2c]; u32 flags; };
extern struct P *gUnk_020004A8;
u32 sub_0821ABA8(s32, s32);

void sub_080727C4(void)
{
    struct P *p = gUnk_020004A8;
    if (p != 0)
        p->flags |= (u16)sub_0821ABA8(0x73, 0);
}
