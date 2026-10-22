#ifndef GUARD_SCALAR_APPROACH_H
#define GUARD_SCALAR_APPROACH_H
#include "global.h"
/* Period-256 angles; the +128 tie remains positive. Arithmetic wraps in
 * u32 before signed interpretation, including the ROM's INT_MIN negation. */
extern const s16 gUnk_08614D4C[];
/* The original only clamps the upper bound. Integer address arithmetic
 * retains its access to preceding mapped ROM for a negative profile index. */
static inline s32 ScalarApproach_ProfileCoefficient(s32 profile)
{
    u32 address = (u32)gUnk_08614D4C;
    {
        u32 offset = (u32)profile * 2u;
        const s16 *entry = (const s16 *)(offset + address);
        return *entry;
    }
}
static inline s32 ScalarApproach_Unwrap(s32 value, s32 center)
{
    s16 delta = ((u32)value - (u32)center) & 255;
    if (delta > 128)
        delta = delta - 256;
    return (s32)((u32)center + (s16)delta);
}
static inline s32 ScalarApproach_Half(s32 current, s32 target)
{
    s32 delta = (s32)((u32)target - (u32)current);
    s32 step;
    if (((u32)delta + 1u) <= 2)
        return target;
    if (delta >= 0)
        step = delta >> 1;
    else
        step = (s32)(0u - (u32)((s32)(0u - (u32)delta) >> 1));
    return (s32)((u32)current + (u32)step);
}
static inline s32 ScalarApproach_Shift2(s32 current, s32 target)
{
    s32 delta = (s32)((u32)target - (u32)current);
    s32 step;
    if (delta != 0) {
        if (((u32)delta + 3u) <= 6) {
            s32 minimum = -4;
            if (delta > 0)
                minimum = 4;
            delta = minimum;
        }
        if (delta >= 0)
            step = delta >> 2;
        else
            step = (s32)(0u - (u32)((s32)(0u - (u32)delta) >> 2));
        target = (s32)((u32)current + (u32)step);
    }
    return target;
}
static inline s32 ScalarApproach_Shift3(s32 current, s32 target)
{
    s32 delta = (s32)((u32)target - (u32)current);
    s32 step;
    if (delta != 0) {
        if (((u32)delta + 7u) <= 14) {
            s32 minimum = -8;
            if (delta > 0)
                minimum = 8;
            delta = minimum;
        }
        if (delta >= 0)
            step = delta >> 3;
        else
            step = (s32)(0u - (u32)((s32)(0u - (u32)delta) >> 3));
        target = (s32)((u32)current + (u32)step);
    }
    return target;
}
static inline s32 ScalarApproach_Shift4(s32 current, s32 target)
{
    s32 delta = (s32)((u32)target - (u32)current);
    s32 step;
    if (delta != 0) {
        if (((u32)delta + 15u) <= 30) {
            s32 minimum = -16;
            if (delta > 0)
                minimum = 16;
            delta = minimum;
        }
        if (delta >= 0)
            step = delta >> 4;
        else
            step = (s32)(0u - (u32)((s32)(0u - (u32)delta) >> 4));
        target = (s32)((u32)current + (u32)step);
    }
    return target;
}
static inline s32 ScalarApproach_Linear(s32 current, s32 target, s32 step)
{
    if (current < target) {
        step = (s32)((u32)current + (u32)step);
        if (step < target)
            goto useStep;
        return target;
    } else {
        step = (s32)((u32)current - (u32)step);
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
        factor = (s32)(0u - (u32)((s32)(0u - (u32)coefficient) >> 8));
    return (s32)(((u32)target - (u32)current) * (u32)factor + (u32)current);
}
/* Same result as ScalarApproach_Linear; this layout matches the hidden entries. */
static inline s32 ScalarApproach_Step(s32 current, s32 target, s32 step)
{
    s32 n, r;
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
#endif
