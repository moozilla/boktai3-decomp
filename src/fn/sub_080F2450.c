#include "global.h"

struct S {
    u8 filler[0x64];
    u16 h64;
    u16 h66;
    u16 h68;
    u16 h6a;
    u16 h6c;
    u16 h6e;
    u16 h70;
    u8 f3[0xcf8 - 0x72];
    u32 cnt;
};

void sub_080F2450(struct S *s)
{
    s->h64 = 0;
    s->h66 = 0;
    s->h68 = 0;
    s->h6c = 0;
    s->h6e = 1;
    s->h70 = 0;
    s->cnt = 0x14;
}
