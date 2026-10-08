#include "global.h"

struct S { u8 f[6]; s16 fl; };
struct O { u8 f0[0x48]; struct S s; u8 g[0xae]; s16 hfc; u8 g2[3]; u8 b100; };
void sub_081B7954(void *);

void sub_081B79F8(struct O *p)
{
    struct S *s = &p->s;
    s32 m = ~4;
    s->fl &= m;
    if (*(u8 *)((u8 *)p + 0x100) == 0 && *(s16 *)((u8 *)p + 0xfc) <= 0)
        sub_081B7954(p);
}
