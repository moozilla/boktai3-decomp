#include "global.h"
/* Math_StepAngleTowardTarget: Steps along the shortest angular difference without overshooting the target */

static inline s32 AngleNear(s32 angle, s32 ref)
{
    s16 d = (angle - ref) & 0xff;
    if (d > 128)
        d -= 256;
    return ref + d;
}

s32 Math_StepAngleTowardTarget(s32 angle, s32 target, s32 step)
{
    s32 cur = AngleNear(angle, target);
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
