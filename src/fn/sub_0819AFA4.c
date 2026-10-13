#include "global.h"

s32 sub_0819AFA4(s32 v, s32 center, s32 range)
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
