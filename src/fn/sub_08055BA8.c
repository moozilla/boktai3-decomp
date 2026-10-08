#include "global.h"

struct S { u8 f[0x1c4]; u32 a; u32 b; };
void sub_08249240(u32, struct S *, u32);

void sub_08055BA8(struct S *p)
{
    if (p->a)
        sub_08249240(p->b, p, p->a);
}
