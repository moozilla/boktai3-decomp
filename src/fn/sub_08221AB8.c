#include "scalar_approach.h"

s32 sub_08221AB8(s32 current, s32 target, s32 step)
{
    s32 n, r;
    current = ScalarApproach_Unwrap(current, target);
    if (current < target) {
        n = current + step;
        if (n < target)
            goto set;
        r = target;
    } else {
        n = current - step;
        r = target;
        if (n > r)
        set:
            r = n;
    }
    return r;
}
