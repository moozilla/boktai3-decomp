#include "engine/math/scalar_approach.h"
void sub_08221FEC(s16 *values, const s16 *targets, s32 count, s32 step)
{
    s16 *value = values;
    const s16 *target = targets;

    while (--count >= 0) {
        *value = ScalarApproach_Step(ScalarApproach_Unwrap(*value, *target), *target, step);
        value++;
        target++;
    }
}