#include "global.h"
struct S { u8 f[0x18]; u8 g[4]; u8 b1c; };
void sub_082195E0(void *);
void sub_081C0378(struct S *p)
{
    if (p->b1c != 0) sub_082195E0(p->f + 0x18);
}
