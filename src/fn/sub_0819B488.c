/* Hresvelgr-local copy: same code, its own profile table. */
#define gUnk_08614D4C gUnk_08611D24
#include "scalar_approach.h"
/* ScalarApproach_ApplyProfileArray: Applies one indexed scalar coefficient profile to each value and target pair */
#define ScalarApproach_ApplyProfileArray sub_0819B488
void ScalarApproach_ApplyProfileArray(s16 *values, const s16 *targets, s32 profile, s32 count)
{
    s16 *current;
    const s16 *target;
    if (profile > 15)
        profile = 15;
    profile = ScalarApproach_ProfileCoefficient(profile);
    current = values;
    target = targets;
    while ((count = (s32)((u32)count - 1u)) >= 0) {
        *current = ScalarApproach_Profile(*current, *target, profile);
        ++current;
        ++target;
    }
}
