#include "global.h"
int sub_081FBEF8(void *, void *, u32);
extern u32 gUnk_020005E4;
struct S { u8 f0[0x18]; u32 mask; u8 f1[0x20]; u8 e[24][0x78]; };
u32 sub_081FC16C(struct S *p) {
    u8 *e = p->f1 + 0x20 - 0x20 + 0;
    s32 i;
    e = (u8 *)p + 0x38;
    for (i = 0; i <= 0x1F; i++, e += 0x78) {
        u32 one = 1 << i;
        if (p->mask & one) sub_081FBEF8(p, e, i);
    }
    return gUnk_020005E4 = 0;
}
