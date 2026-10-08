#include "global.h"

struct S {
    u8 filler[0x1e];
    u16 h1e;
    u8 f2[0xf2 - 0x20];
    u16 hf2;
    u8 f3[0xfa - 0xf4];
    u16 hfa;
    u8 f4[0x180 - 0xfc];
    u32 cnt;
};

void sub_080F49BC(struct S *s)
{
    u32 *c = &s->cnt;
    if ((*c & 3) == 0)
        s->hf2 += s->hfa;
    *c = *c - 1;
    if (*c == 0) {
        s->hfa = -s->hfa;
        *c = 0x28;
    }
    s->h1e += s->hf2;
}
