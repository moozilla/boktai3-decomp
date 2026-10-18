#include "scalar_approach.h"
/* ScalarApproach_ApplyProfile: Applies the indexed scalar coefficient profile with zero coefficient snapping to target */
#define ScalarApproach_ApplyProfile sub_082218C8
#define ScalarApproach_ApplyProfile sub_082218C8
s32 ScalarApproach_ApplyProfile(s32 current, s32 target, s32 profile)
{

    if (profile > 15)
        profile = 15;
    profile = ScalarApproach_ProfileCoefficient(profile);
    return ScalarApproach_Profile(current, target, profile);
}
