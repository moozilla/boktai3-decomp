// MATCHING_CONCESSION: register-pinning
#include "scalar_approach.h"
/* ScalarApproach_ApplyProfileArray: Applies one indexed scalar coefficient profile to each value and target pair */
#define ScalarApproach_ApplyProfileArray sub_08221D40
#define ScalarApproach_ApplyProfileArray sub_08221D40
void ScalarApproach_ApplyProfileArray(s16 *values, const s16 *targets, s32 profile, s32 count)
{
    s16 *cursor = values;
    asm("" : "+r"(cursor));
    if (profile > 15)
        profile = 15;
    profile = ScalarApproach_ProfileCoefficient(profile);
    while ((count = (s32)((u32)count - 1u)) >= 0) {
        *cursor = ScalarApproach_Profile(*cursor, *targets, profile);
        ++cursor;
        ++targets;
    }
}
