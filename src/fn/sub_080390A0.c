#include "global.h"

struct S { u8 f[0xd0]; u32 a; };
extern u32 gUnk_020000FC;
void sub_08033778(u32);
void sub_082156B8(u32);
u32 sub_080390A0(struct S *p)
{
    sub_08033778(p->a);
    sub_082156B8(0);
    sub_082156B8(2);
    return gUnk_020000FC = 0;
}
