#include "engine/math/scalar_approach.h"
extern const s16 gUnk_08611D24[];
/* ScalarApproach_ApplyProfileArray: Applies one indexed scalar coefficient profile to each value and target pair */
void ScalarApproach_ApplyProfileArray(s16 *values, const s16 *targets, s32 profile, s32 count)
{
    s16 *current;
    const s16 *target;
    if (profile > 15)
        profile = 15;
    profile = gUnk_08611D24[profile];
    current = values;
    target = targets;
    while (--count >= 0) {
        *current = ScalarApproach_Profile(*current, *target, profile);
        ++current;
        ++target;
    }
}
