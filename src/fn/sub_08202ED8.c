#include "global.h"
struct S { u16 h0; u16 h2; u8 p4[2]; u8 b6; u8 p7; u32 w8; };
void sub_08202ED8(struct S *p, u32 a, u32 b, u32 c, u32 d)
{
    p->h0 = a;
    p->h2 = b;
    p->b6 = c;
    p->w8 = d;
}
