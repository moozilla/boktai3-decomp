#include "global.h"

static inline s32 AngleNear(s32 angle, s32 ref)
{
    s16 d = (angle - ref) & 0xff;
    s32 x = d;
    if (x > 128)
        d = x - 256;
    return ref + d;
}

static inline s32 ClampAround(s32 v, s32 center, s32 range)
{
    s32 limit = center - range;
    if (v < limit)
        goto clamp;
    limit = center + range;
    if (v > limit)
    clamp:
        v = limit;
    return v;
}

s32 sub_0819AFB8(s32 angle, s32 ref, s32 range)
{
    return ClampAround(AngleNear(angle, ref), ref, range);
}
