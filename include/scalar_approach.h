#ifndef GUARD_SCALAR_APPROACH_H
#define GUARD_SCALAR_APPROACH_H
#include "global.h"
/* Period-256 angles; the +128 tie remains positive. Scalar additions use
 * the target's 32-bit wrapping arithmetic. Array counts must be nonnegative. */
extern const s16 gUnk_08614D4C[];
static inline s32 ScalarApproach_Unwrap(s32 value, s32 center)
{
    s16 delta = ((u32)value - (u32)center) & 255;
    if (delta > 128)
        delta = delta - 256;
    return (s32)((u32)center + (s16)delta);
}
static inline s32 ScalarApproach_Half(s32 current, s32 target)
{
    s32 delta = target - current;
    s32 step;
    if ((u32)(delta + 1) <= 2)
        return target;
    if (delta >= 0)
        step = delta >> 1;
    else
        step = -(-delta >> 1);
    return current + step;
}
static inline s32 ScalarApproach_Shift2(s32 current, s32 target)
{
    s32 delta = target - current;
    s32 step;
    if (delta != 0) {
        if ((u32)(delta + 3) <= 6) {
            s32 minimum = -4;
            if (delta > 0)
                minimum = 4;
            delta = minimum;
        }
        if (delta >= 0)
            step = delta >> 2;
        else
            step = -(-delta >> 2);
        target = current + step;
    }
    return target;
}
static inline s32 ScalarApproach_Shift3(s32 current, s32 target)
{
    s32 delta = target - current;
    s32 step;
    if (delta != 0) {
        if ((u32)(delta + 7) <= 14) {
            s32 minimum = -8;
            if (delta > 0)
                minimum = 8;
            delta = minimum;
        }
        if (delta >= 0)
            step = delta >> 3;
        else
            step = -(-delta >> 3);
        target = current + step;
    }
    return target;
}
static inline s32 ScalarApproach_Shift4(s32 current, s32 target)
{
    s32 delta = target - current;
    s32 step;
    if (delta != 0) {
        if ((u32)(delta + 15) <= 30) {
            s32 minimum = -16;
            if (delta > 0)
                minimum = 16;
            delta = minimum;
        }
        if (delta >= 0)
            step = delta >> 4;
        else
            step = -(-delta >> 4);
        target = current + step;
    }
    return target;
}
static inline s32 ScalarApproach_Linear(s32 current, s32 target, s32 step)
{
    if (current < target) {
        step = current + step;
        if (step < target)
            goto useStep;
        return target;
    } else {
        step = current - step;
        if (step <= target)
            return target;
    }
useStep:
    return step;
}
static inline s32 ScalarApproach_Profile(s32 current, s32 target, s32 coefficient)
{
    s32 factor;
    if (!coefficient)
        return target;
    if (coefficient >= 0)
        factor = coefficient >> 8;
    else
        factor = -(-coefficient >> 8);
    return (target - current) * factor + current;
}
#endif
