#include "global.h"

struct S { u8 f[0x4fc]; u16 a; u16 b; };

void sub_08057960(struct S *p, u16 v)
{
    p->a = v;
    p->b = 0;
}
