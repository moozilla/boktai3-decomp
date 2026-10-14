#include "global.h"

s32 Div(s32, s32);

static inline s32 AngleNear(s32 angle, s32 ref)
{
    s16 d = (angle - ref) & 0xff;
    s32 x = d;
    if (x > 128)
        d = x - 256;
    return ref + d;
}

static inline s32 Shift6(s32 x)
{
    if (x >= 0)
        return x >> 6;
    return -((-x) >> 6);
}

s32 sub_0819B190(s32 angle, s32 target, s32 frames)
{
    s32 cur = AngleNear(angle, target);
    s32 q = Div((target - cur) << 6, frames);
    if ((q < 0 ? -q : q) <= 5)
        return target;
    if ((u32)(q - 1) <= 62)
        q = 64;
    else if (q < 0 && q > -64)
        q = -64;
    return cur + Shift6(q);
}
