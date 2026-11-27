#include "global.h"

s32 Math_StepTowardTarget(s32 cur, s32 target, s32 step)
{
    s32 next, r;
    if (cur < target) {
        next = cur + step;
        if (next < target)
            goto use_next;
        r = target;
    } else {
        next = cur - step;
        r = target;
        if (next > r)
        use_next:
            r = next;
    }
    return r;
}
