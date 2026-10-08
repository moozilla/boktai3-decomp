#include "global.h"

struct S { u8 f[6]; s16 fl; };
struct O { u8 f0[0x48]; struct S s; u8 g[0xa8]; u16 hf8; u16 hfa; };
void sub_081B78CC(void *);

void sub_081B7978(struct O *p)
{
    struct S *s = &p->s;
    s32 m = ~4;
    s->fl &= m;
    if (p->hf8 >= p->hfa)
        sub_081B78CC(p);
}
