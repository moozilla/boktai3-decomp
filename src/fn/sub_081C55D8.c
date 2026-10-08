#include "global.h"

struct S { u8 pad[0x2e1]; u8 f2e1; u8 pad2[0x30c-0x2e2]; u32 f30c; u32 f310; };

void sub_081C55D8(struct S *p, u32 v)
{
    p->f30c = v;
    p->f310 = 0;
    p->f2e1 = 1;
}
