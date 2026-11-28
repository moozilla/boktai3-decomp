#include "engine/math/scalar_approach.h"
/* ScalarApproach_UnwrapAngleArray: Rewrites each signed halfword angle to its nearest representative relative to the corresponding target */
void ScalarApproach_UnwrapAngleArray(s16 *values, const s16 *targets, s32 count)
{
    while (--count >= 0) {
        *values = ScalarApproach_Unwrap(*values, *targets);
        values++;
        targets++;
    }
}
