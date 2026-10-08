#include "global.h"
struct S { u8 f[0xac]; u32 a; };
void sub_081C4950(void *);
void sub_0824923C(void *, u32);
void sub_081C49C0(struct S *p)
{
    sub_081C4950(p);
    if (p->a != 0) sub_0824923C(&p->a, p->a);
}
