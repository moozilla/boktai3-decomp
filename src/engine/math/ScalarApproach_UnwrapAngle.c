#include "engine/math/scalar_approach.h"
/* ScalarApproach_UnwrapAngle: Returns the period-256 angle representative nearest the center with a positive half-turn tie */
s32 ScalarApproach_UnwrapAngle(s32 value, s32 center)
{
    return ScalarApproach_Unwrap(value, center);
}
