#include "scalar_approach.h"
#define ScalarApproach_ApplyAngleProfile sub_08221AFC
s32 ScalarApproach_ApplyAngleProfile(s32 current, s32 target, s32 profile)
{

    if (profile > 15)
        profile = 15;
    profile = gUnk_08614D4C[profile];
    {
        s32 near = ScalarApproach_Unwrap(current, target);
        return ScalarApproach_Profile(near, target, profile);
    }
}
