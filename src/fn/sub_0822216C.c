#include "scalar_approach.h"
/* ScalarApproach_ApplyAngleProfileArray: Unwraps each period-256 angle relative to its target and applies the selected interpolation profile */
#define ScalarApproach_ApplyAngleProfileArray sub_0822216C
#define ScalarApproach_ApplyAngleProfileArray sub_0822216C
void ScalarApproach_ApplyAngleProfileArray(s16 *values, const s16 *targets, s32 profile, s32 count)
{
    s16 *cursor;
    if (profile > 15) {
        profile = 15;
    }
    profile = ScalarApproach_ProfileCoefficient(profile);
    cursor = values;
    while ((count = (s32)((u32)count - 1u)) >= 0) {
        *cursor =
            ScalarApproach_Profile(ScalarApproach_Unwrap(*cursor, *targets), *targets, profile);
        ++cursor;
        ++targets;
    }
}
