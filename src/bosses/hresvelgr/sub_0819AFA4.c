#include "global.h"
/* Math_ClampAroundCenter: Clamps a signed value to the interval centered on center with the given range */

s32 Math_ClampAroundCenter(s32 v, s32 center, s32 range)
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
