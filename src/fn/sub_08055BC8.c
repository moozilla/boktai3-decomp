#include "global.h"

struct S { u8 f[0x1cc]; u32 v; };
void sub_0821AD08(u32, u32);

void sub_08055BC8(struct S *p)
{
    if (p->v)
        sub_0821AD08(p->v, 0);
}
