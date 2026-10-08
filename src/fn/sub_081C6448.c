#include "global.h"

struct S { u8 pad[0x18]; u8 a18[0x40]; u8 a58[0x20]; s16 f78; s16 f7a; u8 pad2[0xb8-0x7c]; u8 ab8[0x20]; u8 pad3[0x358-0xd8]; u32 f358; };
s32 sub_081C5EA8(u32);
void sub_0821980C(u8 *, u8 *, u16, u32);

void sub_081C6448(struct S *p)
{
    s32 r = sub_081C5EA8(p->f358);
    if (r >= 0) {
        sub_0821980C(p->a58, p->a18, r, 0);
        p->f78 = 0x6f;
        p->f7a = -26;
        sub_0821980C(p->ab8, p->a18, 0x5d, 0);
    }
}
