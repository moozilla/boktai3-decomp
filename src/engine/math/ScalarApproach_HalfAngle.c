#include "engine/math/scalar_approach.h"
/* ScalarApproach_HalfAngle: Approaches the target along the wrapped angle difference by half and snaps within one unit */
s32 ScalarApproach_HalfAngle(s32 current, s32 target)
{
    return ScalarApproach_Half(ScalarApproach_Unwrap(current, target), target);
}
