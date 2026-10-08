#include "global.h"
struct S { u8 f[0x36c]; u32 a; };
void sub_081C61BC(void *);
void sub_081C61AC(void *);
void sub_0824923C(void *, u32);
void sub_081C61CC(struct S *p)
{
    sub_081C61BC(p);
    sub_081C61AC(p);
    if (p->a != 0) sub_0824923C(p, p->a);
}
