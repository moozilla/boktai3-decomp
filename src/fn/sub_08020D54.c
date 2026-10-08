#include "global.h"

struct S { u8 f[3]; u8 a; };
u32 sub_08020D54(struct S *p, struct S *q)
{
    u8 v = q->a;
    u32 r;
    if (v == 0) {
        r = 0;
    } else {
        p->a = v;
        r = 1;
    }
    return r;
}
