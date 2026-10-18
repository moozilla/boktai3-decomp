#include "scalar_approach.h"
/* ScalarApproach_ClampCentered: Clamps a scalar to the interval defined by center and radius */
#define ScalarApproach_ClampCentered sub_0822185C
#define ScalarApproach_ClampCentered sub_0822185C
s32 ScalarApproach_ClampCentered(s32 value, s32 center, s32 radius)
{
    s32 bound = (s32)((u32)center - (u32)radius);
    if (value < bound || (bound = (s32)((u32)center + (u32)radius), value > bound))
        value = bound;
    return value;
}
