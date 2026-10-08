#include "global.h"
struct S { u8 p[4]; u16 h4; u8 p6[2]; u32 w8; };
u32 sub_081F34C8(void);
u32 sub_0815F53C(u32);
void sub_081FE98C(struct S *p, u32 n)
{
    p->h4 = n;
    if (n == 1)
        p->w8 = sub_081F34C8();
    else if (n == 2)
        p->w8 = sub_0815F53C(0);
}
