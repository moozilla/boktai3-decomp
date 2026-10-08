#include "global.h"
int sub_0820D2A0(void *, void *, u32);
extern u32 gUnk_02000604;
struct S { u8 f0[0x18]; u32 mask; u8 f1[0x20]; u8 e[24][0xB0]; };
u32 sub_0820D5AC(struct S *p) {
    u8 *e = p->f1 + 0x20 - 0x20 + 0;
    s32 i;
    e = (u8 *)p + 0x1C;
    for (i = 0; i <= 0xF; i++, e += 0xB0) {
        u32 one = 1 << i;
        if (p->mask & one) sub_0820D2A0(p, e, i);
    }
    return gUnk_02000604 = 0;
}
