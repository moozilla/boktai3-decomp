#include "global.h"

static inline s32 StepToward(s32 cur, s32 target, s32 step)
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

static inline void StepArray(s32 step, s16 *cur, s16 *target, s32 count)
{
    while (--count >= 0) {
        *cur = StepToward(*cur, *target, step);
        cur++;
        target++;
    }
}

void sub_0819B44C(s16 *cur, s16 *target, s32 step, s32 count)
{
    StepArray(step, cur, target, count);
}
