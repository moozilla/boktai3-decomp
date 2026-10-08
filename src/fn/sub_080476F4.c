#include "global.h"

struct S { u8 f[0x1c]; u8 a; u8 b; u8 c[10]; u32 d; };
void sub_080476F4(struct S *p, u8 v)
{
    p->a = v;
    p->b = 1;
    p->d = 0;
}
