#include "global.h"

struct S { u8 f[8]; u32 a; };
u32 sub_0803749C(u32 a, struct S *p)
{
    p->a |= 1;
}
