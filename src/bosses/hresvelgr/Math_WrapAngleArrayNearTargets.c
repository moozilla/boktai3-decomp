#include "global.h"
/* Math_WrapAngleArrayNearTargets: Replaces each angle with its equivalent nearest the corresponding target without approaching it */

static inline s32 AngleNear(s32 angle, s32 ref)
{
    s16 d = (angle - ref) & 0xff;
    if (d > 128)
        d -= 256;
    return ref + d;
}

void Math_WrapAngleArrayNearTargets(s16 *cur, s16 *target, s32 count)
{
    while (--count >= 0) {
        *cur = AngleNear(*cur, *target);
        cur++;
        target++;
    }
}
