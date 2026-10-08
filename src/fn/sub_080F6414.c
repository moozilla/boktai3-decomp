#include "global.h"

struct S080F6414 {
    u8 filler0[0x9c]; s16 h;
    u8 filler1[0xb74 - 0x9e]; u8 *q;
    u8 filler2[0xcec - 0xb78]; u32 z;
    u8 filler3[0xd56 - 0xcf0]; u16 w;
};
void sub_08215284(u8 *, u32);

void sub_080F6414(struct S080F6414 *s)
{
    s16 v = --s->h;
    if (v == 0) {
        u8 **q = &s->q;
        sub_08215284(*q, s->w);
        *(u8 **)(*q + 0xc) = (u8 *)s + 0xd30;
        s->z = v;
    }
}
