#include "scalar_approach.h"
/* ScalarApproach_ApplyAngleProfile: Unwraps the current angle relative to target then applies the indexed coefficient profile */
#define ScalarApproach_ApplyAngleProfile sub_08221AFC
#define ScalarApproach_ApplyAngleProfile sub_08221AFC
s32 ScalarApproach_ApplyAngleProfile(s32 current, s32 target, s32 profile)
{

    if (profile > 15)
        profile = 15;
    profile = ScalarApproach_ProfileCoefficient(profile);
    {
        s32 near = ScalarApproach_Unwrap(current, target);
        return ScalarApproach_Profile(near, target, profile);
    }
}
