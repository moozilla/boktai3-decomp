#include "global.h"

s32 Div(s32, s32);
void sub_0805F374(s32 n, s32 *out)
{
    s32 q;
    out[0] = q = Div(n, 100);
    n -= q * 100;
    out[1] = q = Div(n, 10);
    n -= q * 10;
    out[2] = n;
}