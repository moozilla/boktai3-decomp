#include "engine/math/scalar_approach.h"
/* ScalarApproach_ApplyAngleProfileArray_0822216C: Unwraps each period-256 angle relative to its target and applies the selected interpolation profile */
void ScalarApproach_ApplyAngleProfileArray_0822216C(s16 *values, const s16 *targets, s32 profile,
                                                    s32 count)
{
    s16 *cursor;
    if (profile > 15) {
        profile = 15;
    }
    profile = ScalarApproach_ProfileCoefficient(profile);
    cursor = values;
    while ((count = (s32)((u32)count - 1)) >= 0) {
        *cursor =
            ScalarApproach_Profile(ScalarApproach_Unwrap(*cursor, *targets), *targets, profile);
        ++cursor;
        ++targets;
    }
}
