#include "global.h"

struct S {
    u8 filler[0x382];
    u16 b;
    u32 a;
};

void sub_08052628(struct S *p, u32 v)
{
    p->a = v;
    p->b = 0;
}
