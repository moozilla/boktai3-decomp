#include "global.h"
struct S { u8 f[0x30c]; u32 a; };
void sub_081C58BC(void *);
void sub_0824923C(void *, u32);
void sub_081C59A4(struct S *p)
{
    sub_081C58BC(p);
    if (p->a != 0) sub_0824923C(p, p->a);
}
