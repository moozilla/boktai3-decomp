#include "global.h"

struct S { u8 f[0x1a]; u8 a; u8 g[0x41]; void *b; u8 h[0x164]; u32 idx; u8 i[0x18]; u16 v; };
s32 sub_08031E34(void *);
s32 sub_080064C4(void *);
u32 sub_082282A4(u32);
u32 sub_080066C0(struct S *p)
{
    if (p->b != 0 && sub_08031E34(p->b) != 0) {
        if (p->a == 0) {
            if (sub_080064C4(p) == 1)
                return 0;
        } else {
            p->a--;
        }
        p->v = sub_082282A4(p->idx);
        p->idx = (p->idx + 1) & 0x3f;
    }
    return 0;
}
