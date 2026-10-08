#include "global.h"
s32 Div(s32, s32);
void sub_08165C08(s16 *a, s16 *b, s32 n)
{
    s32 m;
    a[0] = Div(a[0] * n + b[0], m = n + 1);
    a[1] = Div(a[1] * n + b[1], m);
}
