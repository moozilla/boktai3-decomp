#include "engine/math/scalar_approach.h"
/* ScalarApproach_ApplyProfileArray_08221D40: Applies one indexed scalar coefficient profile to each value and target pair */
void ScalarApproach_ApplyProfileArray_08221D40(s16 *values, const s16 *targets, s32 profile,
                                               s32 count)
{
    if (profile > 15) {
        profile = 15;
    }
    profile = ScalarApproach_ProfileCoefficient(profile);
    while ((count = (s32)((u32)count - 1)) >= 0) {
        *values = ScalarApproach_Profile(*values, *targets, profile);
        do {
            ++values;
        } while (0);
        ++targets;
    }
}
