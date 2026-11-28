#include "engine/math/scalar_approach.h"
extern const s16 gUnk_08611D24[];
/* ScalarApproach_ApplyAngleProfileArray: Unwraps each period-256 angle relative to its target and applies the selected interpolation profile */
void ScalarApproach_ApplyAngleProfileArray(s16 *values, const s16 *targets, s32 profile, s32 count)
{
    s16 *current;
    const s16 *target;
    if (profile > 15)
        profile = 15;
    profile = gUnk_08611D24[profile];
    current = values;
    target = targets;
    while (--count >= 0) {
        *current =
            ScalarApproach_Profile(ScalarApproach_Unwrap(*current, *target), *target, profile);
        ++current;
        ++target;
    }
}
