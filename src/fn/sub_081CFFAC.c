#include "global.h"

extern u32 gUnk_08612568[];
struct S { u8 pad[5]; u8 b5; u8 b6; u8 pad7[2]; u8 b9; u8 pad2[6]; u32 f10; u8 pad3[4]; u8 b18; u8 pad4[0x2cc-0x19]; u32 f2cc; };

s32 sub_081CFFAC(struct S *p)
{
    s32 r;
    if ((u8)(p->b5 - 1) <= 0x21) {
        u32 t = gUnk_08612568[p->b5];
        u8 v = p->b5;
        s32 z = 0;
        p->b6 = v;
        p->f2cc = t;
        p->f10 = z;
        p->b9 = 1;
        p->b18 = z;
        r = 1;
    } else {
        r = 0;
    }
    return r;
}
