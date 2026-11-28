#include "effects/bursts/radial_burst.h"

void RadialBurstEffect_BlendSpritePalette(struct RadialBurstFade *fade)
{
    const u16 *source = fade->source;
    s32 bias = (16 - fade->alpha) * 15;
    s32 i = 0;
    u32 mask = 31;

    for (; i < 16; source++, i++) {
        u32 color = *source;
        s32 r = color & 31;
        s32 g = ((color << 16) >> 21) & mask;
        s32 b = ((color << 16) >> 26) & mask;

        r = (bias + r * fade->alpha) >> 4;
        g = (bias + g * fade->alpha) >> 4;
        b = (bias + b * fade->alpha) >> 4;
        fade->colors[i] = (b << 10) | (g << 5) | r;
    }
}
