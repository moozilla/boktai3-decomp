#include "global.h"

struct S { u8 f[0x50]; u32 a; };
void sub_0802C8A0(struct S *p)
{
    u32 m = 1;
    p->a |= m;
}
