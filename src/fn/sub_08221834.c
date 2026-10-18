#include "scalar_approach.h"
/* ScalarApproach_UnwrapAngle: Returns the period-256 angle representative nearest the center with a positive half-turn tie */
#define ScalarApproach_UnwrapAngle sub_08221834
#define ScalarApproach_UnwrapAngle sub_08221834
s32 ScalarApproach_UnwrapAngle(s32 value, s32 center)
{
    return ScalarApproach_Unwrap(value, center);
}
