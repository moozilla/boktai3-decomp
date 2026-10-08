#include "global.h"
struct S { u8 f[0x50]; u32 cb; };
void sub_081C069C(void *);
void sub_0824923C(void *, u32);
void sub_081C0A80(struct S *p)
{
    sub_081C069C(p);
    if (p->cb != 0) sub_0824923C(p, p->cb);
}
