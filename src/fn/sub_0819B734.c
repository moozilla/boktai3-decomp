#include "scalar_approach.h"
void sub_0819B734(s16 *values, const s16 *targets, s32 count, s32 step)
{
    s16 *v = values;
    const s16 *t = targets;
    while (--count >= 0) {
        *v = ScalarApproach_Step(ScalarApproach_Unwrap(*v, *t), *t, step);
        v++;
        t++;
    }
}