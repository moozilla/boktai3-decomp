#ifndef GUARD_VANARGAND_STAGE_COLORS_H
#define GUARD_VANARGAND_STAGE_COLORS_H

#include "render.h"

static inline s32 Vanargand_InterpolateStageChannel(s32 start, s32 end, s32 frame, s32 duration)
{
    s32 value;
    if (frame <= 0)
        return start;
    if (frame >= duration)
        return end;
    value = ((end - start) << 8) * frame / duration;
    if (value >= 0)
        value >>= 8;
    else
        value = -(-value >> 8);
    return start + value;
}

static inline void Vanargand_SetStageColors(s32 stage)
{
    s32 red, green, blue;
    u16 color;
    red = Vanargand_InterpolateStageChannel(7, 19, stage, 3);
    green = Vanargand_InterpolateStageChannel(0, 18, stage, 3);
    blue = Vanargand_InterpolateStageChannel(0, 19, stage, 3);
    color = red | (green << 5) | (blue << 10);
    gBgPaletteBuffer[0x56 / 2] = color;
    gBgPaletteBuffer[0x10c / 2] = color;
    {
        s32 red, green, blue;
        u16 nextColor;
        red = Vanargand_InterpolateStageChannel(21, 24, stage, 3);
        green = Vanargand_InterpolateStageChannel(0, 29, stage, 3);
        blue = Vanargand_InterpolateStageChannel(0, 31, stage, 3);
        nextColor = red | (green << 5) | (blue << 10);
        gBgPaletteBuffer[0x5a / 2] = nextColor;
        gBgPaletteBuffer[0x10e / 2] = nextColor;
    }
}

#endif
