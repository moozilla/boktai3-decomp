#include "global.h"

struct P { u32 a; u32 b; };
struct Q { u8 f[0x38]; struct P p38; u8 g[0x58]; struct P p98; s16 ha0; u8 g2[0x102]; u32 w1a4; };
s16 sub_081BAD98(void *);
s32 sub_081BADFC(void *);

void sub_081BAF9C(struct Q *p)
{
    u32 m;
    p->ha0 = sub_081BAD98(p);
    p->p38 = p->p98;
    if (sub_081BADFC(p) == 0) {
        m = 2;
        p->w1a4 |= m;
    }
}
