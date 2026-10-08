#include "global.h"

struct S { u8 f[0x7a4]; u32 a; };
void sub_0824923C(struct S *, u32);
void sub_0801C24C(struct S *);
u32 sub_0801DE98(struct S *p)
{
    sub_0824923C(p, p->a);
    sub_0801C24C(p);
    return 0;
}
