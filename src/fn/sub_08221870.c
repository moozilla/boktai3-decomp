#include "scalar_approach.h"
/* ScalarApproach_ClampAngle: Unwraps a period-256 angle around the center then clamps it to the radius */
#define ScalarApproach_ClampAngle sub_08221870
#define ScalarApproach_ClampAngle sub_08221870
s32 ScalarApproach_ClampAngle(s32 value, s32 center, s32 radius)
{
    s32 bound;
    value = ScalarApproach_Unwrap(value, center);
    bound = (s32)((u32)center - (u32)radius);
    if (value < bound || (bound = (s32)((u32)center + (u32)radius), value > bound))
        value = bound;
    return value;
}
