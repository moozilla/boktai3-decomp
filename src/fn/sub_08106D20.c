#include "global.h"

struct P { u8 f[0x18]; u32 a; u32 b; u8 g[0xa0 - 0x20]; u32 c; u8 d; };
extern struct P *gUnk_020001B4;
u32 sub_0821A520(s32, s32);

s32 sub_08106D20(struct P *p)
{
    p->a = sub_0821A520(0x922e, 0x1003);
    p->b = sub_0821A520(0x922e, 0x931e);
    gUnk_020001B4 = p;
    p->c = 0;
    p->d = 0xff;
    return 0;
}
