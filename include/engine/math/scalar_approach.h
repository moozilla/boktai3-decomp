#ifndef GUARD_SCALAR_APPROACH_H
#define GUARD_SCALAR_APPROACH_H
#include "global.h"
/* Period-256 angles; the +128 tie remains positive. */
extern const s16 gUnk_08614D4C[];
/* Callers only clamp the upper bound: a negative profile index reads the ROM before the table. */
static inline s32 ScalarApproach_ProfileCoefficient(s32 profile)
{
    return gUnk_08614D4C[profile];
}
static inline s32 ScalarApproach_Unwrap(s32 value, s32 center)
{
    s16 delta = (value - center) & 255;
    if (delta > 128)
        delta -= 256;
    return center + delta;
}
static inline s32 ScalarApproach_Half(s32 current, s32 target)
{
    s32 delta = target - current;
    s32 step;
    if ((u32)delta + 1 <= 2)
        return target;
    if (delta >= 0)
        step = delta >> 1;
    else
        step = -(-delta >> 1);
    return current + step;
}
static inline s32 ScalarApproach_ApplyDelta(s32 current, s32 target, s32 delta, u32 shift)
{
    s32 step;
    u32 threshold = (1u << shift) - 1;
    if (delta != 0) {
        if (delta + threshold <= 2 * threshold) {
            s32 minimum = -(1 << shift);
            if (delta > 0)
                minimum = 1 << shift;
            delta = minimum;
        }
        if (delta >= 0)
            step = delta >> shift;
        else
            step = -(-delta >> shift);
        target = current + step;
    }
    return target;
}
static inline s32 ScalarApproach_Shift2(s32 current, s32 target)
{
    return ScalarApproach_ApplyDelta(current, target, target - current, 2);
}
static inline s32 ScalarApproach_Shift3(s32 current, s32 target)
{
    return ScalarApproach_ApplyDelta(current, target, target - current, 3);
}
static inline s32 ScalarApproach_Shift4(s32 current, s32 target)
{
    return ScalarApproach_ApplyDelta(current, target, target - current, 4);
}
static inline void ScalarApproach_AdvanceValue(s16 *value, const s16 *to, u32 shift)
{
    s32 target = *to;
    s32 current = *value;
    *value = ScalarApproach_ApplyDelta(current, target, target - current, shift);
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
/* Move current toward target by at most step. */
static inline s32 ScalarApproach_Step(s32 current, s32 target, s32 step)
{
    s32 next;
    if (current < target) {
        next = current + step;
        if (next >= target)
            return target;
    } else {
        next = current - step;
        if (next <= target)
            return target;
    }
    return next;
}
#endif
