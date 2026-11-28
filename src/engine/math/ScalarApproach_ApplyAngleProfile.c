#include "engine/math/scalar_approach.h"
/* ScalarApproach_ApplyAngleProfile: Unwraps the current angle relative to target then applies the indexed coefficient profile */
s32 ScalarApproach_ApplyAngleProfile(s32 current, s32 target, s32 profile)
{
    if (profile > 15)
        profile = 15;
    profile = ScalarApproach_ProfileCoefficient(profile);
    return ScalarApproach_Profile(ScalarApproach_Unwrap(current, target), target, profile);
}
