#include "global.h"

struct S { u8 f[0x1c]; u8 a; };
void sub_080476F4(struct S *, u32);
void sub_08047B78(struct S *p)
{
    if (p != 0 && p->a != 0)
        sub_080476F4(p, 4);
}
