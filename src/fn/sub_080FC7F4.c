#include "global.h"

struct S080FC7F4 { u8 filler[0x3d0]; u8 *q; };
void sub_0821FF24(u8 *, u8 *, u32);
void sub_0821FE40(u8 *);

void sub_080FC7F4(struct S080FC7F4 *s)
{
    u8 *q = s->q;
    s32 i = 0;
    u8 *k = q + 0xa54;
    for (; i < 3; i++) {
        u8 *e;
        s32 o = i * 0x54;
        sub_0821FF24(q + (o + 0xb8c), k, 0);
        e = q + (o + 0xa90);
        sub_0821FF24(e, k, 0);
        sub_0821FE40(e);
    }
}
