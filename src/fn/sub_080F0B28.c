#include "global.h"

struct R { u8 f[8]; u16 v; };
struct P { u8 f[4]; u8 *q; };

u16 sub_080F0B28(struct P *p)
{
    struct R *r = (struct R *)(p->q + 0x48);
    if (r->v == 3)
        return 1;
    if (r->v == 4)
        return 0;
    return r->v;
}
