#include "engine/math/scalar_approach.h"
/* ScalarApproach_Snap8Array: Steps signed halfword elements one eighth toward targets and snaps when the step magnitude is below the threshold */
void ScalarApproach_Snap8Array(s16 *values, const s16 *targets, s32 threshold, s32 count)
{
    s16 *value = values;
    const s16 *target = targets;

    while (--count >= 0) {
        s32 current = *value;
        s32 to = *target;
        s32 delta = to - current;
        s32 step;

        if (delta >= 0)
            step = delta >> 3;
        else
            step = -(-delta >> 3);
        current += step;
        if (step > -threshold && step < threshold)
            current = to;
        *value = current;
        value++;
        target++;
    }
}
