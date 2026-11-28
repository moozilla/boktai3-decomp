#include "engine/math/scalar_approach.h"
/* ScalarApproach_ClampCentered: Clamps a scalar to the interval defined by center and radius */
s32 ScalarApproach_ClampCentered(s32 value, s32 center, s32 radius)
{
    s32 bound = center - radius;
    if (value < bound || (bound = center + radius, value > bound))
        value = bound;
    return value;
}
