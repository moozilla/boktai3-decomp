#include "engine/math/scalar_approach.h"
/* ScalarApproach_ApplyWrappedProfileArray: Unwraps each period-256 angle relative to its target and applies the selected interpolation profile */
void ScalarApproach_ApplyWrappedProfileArray(s16 *values, const s16 *targets, s32 profile,
                                             s32 count)
{
    s16 *value;

    if (profile > 15)
        profile = 15;
    profile = ScalarApproach_ProfileCoefficient(profile);
    value = values;
    while (--count >= 0) {
        *value = ScalarApproach_Profile(ScalarApproach_Unwrap(*value, *targets), *targets, profile);
        value++;
        targets++;
    }
}
