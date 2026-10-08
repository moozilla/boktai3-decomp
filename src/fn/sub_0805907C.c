#include "global.h"

struct S { u8 f[0x1360]; u32 v; };
void sub_0824923C(struct S *, u32);

void sub_0805907C(struct S *p)
{
    u32 v = p->v;
    if (v)
        sub_0824923C(p, v);
}
