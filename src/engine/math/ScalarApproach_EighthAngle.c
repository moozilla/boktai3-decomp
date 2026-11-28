#include "engine/math/scalar_approach.h"
/* ScalarApproach_EighthAngle: Approaches the target along the wrapped angle difference by one eighth with a minimum unit step */
s32 ScalarApproach_EighthAngle(s32 current, s32 target)
{
    return ScalarApproach_Shift3(ScalarApproach_Unwrap(current, target), target);
}
