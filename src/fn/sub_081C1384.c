#include "global.h"
struct S { u8 f[0x60]; u32 fl; u8 g[8]; u32 a6c; u32 pad; u32 a74; u8 h[0x454 - 0x78]; u32 a454; };
void sub_0824923C(void *, u32);
u32 sub_081C1384(struct S *p)
{
    if ((p->fl & 1) == 0) p->a6c++;
    if ((p->fl & 2) == 0) p->a74++;
    sub_0824923C(p, p->a454);
    return 0;
}
