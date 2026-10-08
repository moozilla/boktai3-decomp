#include "global.h"

struct P { u8 f[0x34]; u32 a; u8 g[0xfc8 - 0x38]; u32 b; u32 c; };
extern struct P *gUnk_0200014C;
void sub_082151E4(u8 *, s32);
u32 sub_0821A520(s32, s32);

s32 sub_08069620(struct P *p)
{
    sub_082151E4((u8 *)p + 0x18, 0xE74B);
    p->a = sub_0821A520(0x922e, 0x871c);
    p->b = 0;
    p->c = 0;
    gUnk_0200014C = p;
    return 0;
}
