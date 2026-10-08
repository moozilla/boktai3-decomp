#include "global.h"
struct S { u8 f[0x5b8]; u32 a; };
void sub_0824923C(void *, u32);
void sub_081C18E8(void *);
void sub_081C2240(void *);
u32 sub_081C2294(struct S *p)
{
    sub_0824923C(p, p->a);
    sub_081C18E8(p);
    sub_081C2240(p);
    return 0;
}
