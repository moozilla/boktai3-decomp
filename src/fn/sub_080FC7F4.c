#include "global.h"

struct S080FC7F4 {
    u8 filler[0x3d0];
    u8 *q;
};
void Collider_SetPosition(u8 *, u8 *, u32);
s32 Collider_Refresh(u8 *);

void sub_080FC7F4(struct S080FC7F4 *s)
{
    u8 *q = s->q;
    s32 i = 0;
    u8 *k = q + 0xa54;
    for (; i < 3; i++) {
        u8 *e;
        s32 o = i * 0x54;
        Collider_SetPosition(q + (o + 0xb8c), k, 0);
        e = q + (o + 0xa90);
        Collider_SetPosition(e, k, 0);
        Collider_Refresh(e);
    }
}
