#include "global.h"

struct S { u8 f[0x18]; s32 a; s32 b; };

s32 sub_08215184(s32);
s32 sub_0821A520(s32, s32);

void sub_081151D0(struct S *p)
{
    p->b = sub_08215184(0x1C1A);
    p->a = sub_0821A520(0x922E, 0xD1B8);
}
