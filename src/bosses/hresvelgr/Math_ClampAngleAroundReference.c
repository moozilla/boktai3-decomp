#include "global.h"
/* Math_ClampAngleAroundReference: Wraps a 256-unit angle near the reference then clamps its deviation */

static inline s32 AngleNear(s32 angle, s32 ref)
{
    s16 d = (angle - ref) & 0xff;
    if (d > 128)
        d -= 256;
    return ref + d;
}

static inline s32 ClampAround(s32 v, s32 center, s32 range)
{
    s32 limit;
    if (v < (limit = center - range) || v > (limit = center + range))
        v = limit;
    return v;
}

s32 Math_ClampAngleAroundReference(s32 angle, s32 ref, s32 range)
{
    return ClampAround(AngleNear(angle, ref), ref, range);
}
