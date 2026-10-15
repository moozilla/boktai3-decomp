#include "scalar_approach.h"
#define ScalarApproach_ClampAngle sub_08221870
s32 ScalarApproach_ClampAngle(s32 value, s32 center, s32 radius)
{
    s32 bound;
    value = ScalarApproach_Unwrap(value, center);
    bound = center - radius;
    if (value < bound || (bound = center + radius, value > bound))
        value = bound;
    return value;
}
