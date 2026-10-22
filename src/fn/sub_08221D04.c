#include "scalar_approach.h"

void sub_08221D04(s16 *values, const s16 *targets, s32 step, s32 count)
{
    s16 *v = values;
    const s16 *t = targets;
    while (--count >= 0) {
        *v = ScalarApproach_Step(*v, *t, step);
        v++;
        t++;
    }
}
