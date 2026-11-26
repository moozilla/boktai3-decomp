#ifndef GUARD_MATH_SHIFT_H
#define GUARD_MATH_SHIFT_H

#include "global.h"

/* Divide by 2^shift rounding toward zero (a plain >> rounds a negative value down). The
   sine-table motion and fixed-point scaling code of nearly every actor does this. */
// concession-ok(literal-param): arithmetic shift toward zero, a general helper called with many different counts (2, 5, 6, 9, 12), not a fixed mask
static inline s32 Math_ShiftTowardZero(s32 n, u32 shift)
{
    if (n >= 0)
        return n >> shift;
    return -((-n) >> shift);
}

#endif
