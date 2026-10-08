#include "global.h"

struct S { u8 f[4]; u16 a; };
void sub_08045D18(struct S *p, u32 v)
{
    p->a |= v;
}
