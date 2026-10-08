#include "global.h"

struct S {
    u8 filler[0xe4];
    u8 a;
    u8 filler2[0xf];
    u16 c;
    u16 d;
    u32 b;
};

void sub_0804F7D0(struct S *p, u8 a, u32 b)
{
    p->a = a;
    p->b = b;
    p->c = 0;
    p->d = 0;
}
