#include "global.h"

s32 Hud_GetMinimum(s32 a, s32 b)
{
    s32 r = b;
    if (a <= r)
        r = a;
    return r;
}
