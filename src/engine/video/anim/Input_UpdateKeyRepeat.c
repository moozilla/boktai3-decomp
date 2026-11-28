#include "global.h"
#include "engine/runtime_input.h"

u32 Input_UpdateKeyRepeat(u32 pad, u8 *count, u32 mask, u32 result, s32 delay)
{
    u32 ret = 0;
    s32 held = *count;

    if (gPadState[pad].pressed & mask)
        held = delay;
    else if (gPadState[pad].held & mask) {
        if (held < delay)
            held++;
    } else
        held = 0;
    if (held >= delay) {
        held = 0;
        ret = result;
    }
    *count = held;
    return ret;
}
