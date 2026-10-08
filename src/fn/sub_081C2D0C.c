#include "global.h"
struct S { u8 f[0x77c]; u32 a; };
void sub_081C2C04(void *);
void sub_081C2BB8(void *);
void sub_0824923C(void *, u32);
void sub_081C2D0C(struct S *p)
{
    sub_081C2C04(p);
    sub_081C2BB8(p);
    if (p->a != 0) sub_0824923C(p, p->a);
}
