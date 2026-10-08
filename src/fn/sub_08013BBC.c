#include "global.h"
struct S { u8 f[0xc]; u32 fl; };
u32 sub_08013BBC(struct S *p, u32 on)
{
    if (on)
        p->fl |= 2;
    else
        p->fl &= ~2;
}
