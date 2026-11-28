#include "engine/math/scalar_approach.h"
/* ScalarApproach_ClampAngle: Unwraps a period-256 angle around the center then clamps it to the radius */
s32 ScalarApproach_ClampAngle(s32 value, s32 center, s32 radius)
{
    s32 bound;
    value = ScalarApproach_Unwrap(value, center);
    bound = center - radius;
    if (value < bound || (bound = center + radius, value > bound))
        value = bound;
    return value;
}
