#include "global.h"

struct S {
    u8 filler[0x66];
    u16 h66;
    u8 f2[0x6e - 0x68];
    u16 h6e;
    u8 f3[0xb86 - 0x70];
    u16 hb86;
    u8 f4[0xcf8 - 0xb88];
    u32 cnt;
};

void sub_080F2480(struct S *s)
{
    u32 *c = &s->cnt;
    if ((*c & 3) == 0)
        s->h66 += s->h6e;
    *c = *c - 1;
    if (*c == 0) {
        s->h6e = -s->h6e;
        *c = 0x28;
    }
    s->hb86 += s->h66;
}
