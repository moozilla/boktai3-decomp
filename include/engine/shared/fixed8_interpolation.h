#ifndef GUARD_FIXED8_INTERPOLATION_H
#define GUARD_FIXED8_INTERPOLATION_H

#include "engine/shared/global.h"

/* Interpolate whole coordinates with eight fractional bits, clamping the endpoints. */
static inline s32 Fixed8_Interpolate(s32 start, s32 end, s32 duration, s32 timer)
{
    s32 scaled;

    if (timer <= 0)
        return start;
    if (timer >= duration)
        return end;
    scaled = ((s32)((u32)(end - start) << 8) * timer) / duration;
    if (scaled >= 0)
        scaled >>= 8;
    else
        scaled = -((-scaled) >> 8);
    return start + scaled;
}

#endif
