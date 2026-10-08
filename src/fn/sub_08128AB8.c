#include "global.h"

struct S { u8 filler[0x18]; u8 a[0x1c]; u32 b; u32 c; };
extern struct S *gUnk_02000170;
void sub_082151E4(u8 *, u32);
u32 sub_0821A520(u32, u32);

s32 sub_08128AB8(struct S *s)
{
    sub_082151E4(s->a, 0x0000D45E);
    s->b = sub_0821A520(0x0000922E, 0x00001DC3);
    s->c = 0;
    gUnk_02000170 = s;
    return 0;
}
