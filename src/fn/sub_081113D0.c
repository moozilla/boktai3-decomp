#include "global.h"
void sub_0824923C(void *, u32);
void sub_081114E8(void *);
struct S { u8 f[0x18]; u32 a; };
u32 sub_081113D0(struct S *p)
{
    if (p->a)
        sub_0824923C(p, p->a);
    sub_081114E8(p);
    return 0;
}
