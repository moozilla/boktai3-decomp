#include "global.h"

struct S { u8 f[0x2c]; u8 t[8]; u8 n; };
s32 sub_08006478(struct S *p, s32 a, s32 d)
{
    s32 x = a;
    s32 i;
    for (i = 0; i < p->n; i++) {
        x += d;
        if (x > p->n)
            x = p->n;
        else if (x < 0)
            x = 0;
        if (p->t[x] != 0)
            return x;
    }
    return a;
}
