#include "global.h"
void sub_081A5394(s16 *);
s32 __divsi3(s32, s32);
static inline s32 shift(s32 x, s32 n)
{
    s32 r;
    if (x >= 0)
        r = x >> n;
    else
        r = -((-x) >> n);
    return r;
}
void sub_081A543C(s16 *p)
{
    // one variable per value, not a and b reused for both passes
    // (found by tools/autosolve.py: perm_split_var)
    s32 a2;
    s32 b1;
    s16 *q2;
    s16 v[3];
    s32 y = p[1];
    s16 *q = v;
    s32 x = p[0], z = p[2];
    s32 a, b, c;
    q[0] = shift((x - z) * 3, 5);
    a = shift((x + z) * 3, 5);
    b1 = shift(p[1] * 3, 5);
    q[1] = a - b1;
    q[2] = a + b1;
    q2 = v;
    sub_081A5394(v);
    a2 = v[0];
    b = q2[1];
    c = __divsi3((a2 + b) << 5, 3) + y;
    p[0] = shift(c, 1);
    p[1] = y;
    c = __divsi3((b - a2) << 5, 3) + y;
    p[2] = shift(c, 1);
}
