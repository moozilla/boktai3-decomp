#include "global.h"

struct S { u8 f[0x148]; u32 a; };
void sub_0802C934(struct S *, u32);
void sub_0802C91C(struct S *p, u32 a, u32 b)
{
    p->a = a;
    sub_0802C934(p, b);
}
