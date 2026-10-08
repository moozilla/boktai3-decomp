#include "global.h"
struct S { u8 f[0xd4]; u32 a; };
void sub_081C5264(void *);
void sub_0824923C(void *, u32);
void sub_081C52DC(struct S *p)
{
    sub_081C5264(p);
    if (p->a != 0) sub_0824923C(p, p->a);
}
