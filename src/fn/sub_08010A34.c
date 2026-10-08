#include "global.h"

struct S { u8 a; u8 f[0x3f]; u32 b; u8 g[0x44]; u8 c; };
void sub_08013440(u8 *);
void sub_08010A34(u32 x, struct S *p)
{
    if (p->a != 0) {
        p->a = 0;
        p->b |= 1;
        sub_08013440((u8 *)p + 0x88);
    }
}
