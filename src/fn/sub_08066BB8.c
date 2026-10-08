#include "global.h"

struct P { u8 f[0x34]; u8 g[0x1c]; u32 a; u32 b; u32 c; };
void sub_082151E4(u8 *, s32);
u32 sub_08215184(s32);
u32 sub_0821A520(s32, s32);

void sub_08066BB8(struct P *p)
{
    sub_082151E4((u8 *)p + 0x18, 0x363c);
    sub_082151E4((u8 *)p + 0x34, 0x363d);
    p->c = sub_08215184(0x1c1c);
    p->a = sub_0821A520(0x922e, 0x6830);
    p->b = sub_0821A520(0x922e, 0xd41f);
}
