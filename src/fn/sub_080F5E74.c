#include "global.h"

struct S080F5E74 {
    u8 filler0[0xc]; u8 *q;
    u8 filler1[0x128 - 0x10]; s16 h;
    u8 filler2[0x160 - 0x12a]; u8 *p;
    u8 filler3[0x168 - 0x164]; u32 z;
};
void sub_08215284(u8 *, u32);

void sub_080F5E74(struct S080F5E74 *s)
{
    s16 v = --s->h;
    if (v == 0) {
        u8 *o = s->p;
        sub_08215284(s->q, *(u16 *)(o + 0xd56));
        *(u8 **)(s->q + 0xc) = o + 0xd30;
        s->z = v;
    }
}
