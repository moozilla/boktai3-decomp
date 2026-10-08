#include "global.h"
struct S { u8 p[0x20]; u16 h20; u8 q[0x50 - 0x22]; u32 w50; u32 w54; };
void sub_08202F2C(struct S *p, u32 a, u32 b, u32 c)
{
    p->w50 = a;
    p->w54 = b;
    p->h20 = c;
}
