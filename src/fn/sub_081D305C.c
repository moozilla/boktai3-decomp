#include "global.h"

struct S { u8 pad[0x3a0]; u32 f3a0; };
void sub_0824923C(struct S *, u32);

void sub_081D305C(struct S *p)
{
    if (p->f3a0) sub_0824923C(p, p->f3a0);
}
