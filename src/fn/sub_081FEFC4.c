#include "global.h"
int sub_081FEDE8(void *, void *, u32);
extern u32 gUnk_020005F0;
struct S { u8 f0[0x18]; u32 mask; u8 f1[0x20]; u8 e[24][0xB8]; };
u32 sub_081FEFC4(struct S *p) {
    u8 *e = p->f1 + 0x20 - 0x20 + 0;
    s32 i;
    e = (u8 *)p + 0x1C;
    for (i = 0; i <= 0x1F; i++, e += 0xB8) {
        u32 one = 1 << i;
        if (p->mask & one) sub_081FEDE8(p, e, i);
    }
    return gUnk_020005F0 = 0;
}
