#include "engine/math/scalar_approach.h"

void sub_08221D04(s16 *values, const s16 *targets, s32 step, s32 count)
{
    s16 *value = values;
    const s16 *target = targets;

    while (--count >= 0) {
        *value = ScalarApproach_Step(*value, *target, step);
        value++;
        target++;
    }
}
