#include "global.h"

struct S { u8 f[0xee]; u16 a; };
u32 sub_080341B0(struct S *p, u16 v)
{
    p->a = v;
}
