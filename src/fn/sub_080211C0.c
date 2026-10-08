#include "global.h"

struct S { u8 f[0x50]; u32 a; u32 b; };
void sub_080211C0(struct S *p)
{
    p->a = 0;
    p->b = 0;
}
