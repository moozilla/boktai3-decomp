#include "global.h"

struct T { u8 f[0x120]; u16 a; u16 b; u8 g[4]; u16 c; };
struct O { u8 f[0x50]; struct T *t; };

s32 sub_081A6EBC(struct O *o, s32 m, s32 v)
{
    struct T *t = o->t;
    if (m == 0) {
        v -= t->b;
        if (v > 0)
            t->c = v;
        t->a = 1;
    } else if (m == 1) {
        t->a = 0;
    }
    return 1;
}
