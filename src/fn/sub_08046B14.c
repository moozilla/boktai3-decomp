#include "global.h"

struct S { u8 f[0x18]; u16 a; u8 b; u8 c; u8 d; u8 e; u32 g; };
void sub_08046B14(struct S *p)
{
    u32 one = 1;
    u32 z = 0;
    p->c = one;
    p->d = one;
    p->g = z;
    p->b = 0xc0;
    p->a = z;
}
