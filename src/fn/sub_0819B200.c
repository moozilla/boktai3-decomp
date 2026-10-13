#include "global.h"

static inline s32 AngleNear(s32 angle, s32 ref)
{
    s16 d = (angle - ref) & 0xff;
    s32 x = d;
    if (x > 128)
        d = x - 256;
    return ref + d;
}

s32 sub_0819B200(s32 angle, s32 target, s32 step)
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
