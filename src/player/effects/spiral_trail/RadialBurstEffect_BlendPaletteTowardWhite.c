#include "global.h"
#include "player/effects/spiral_trail_sprite.h"
/* RadialBurstEffect_BlendPaletteTowardWhite: same code as RadialBurstEffect_BlendSpritePalette at another address (ported by relatives.py; provisional name) */
void RadialBurstEffect_BlendPaletteTowardWhite(struct SpiralTrailSprite *s)
{
    const u16 *source = s->sourcePalette;
    int bias = (16 - s->alpha) * 15;
    s32 i = 0;
    u32 mask = 31;

    for (; i < 16; source++, i++) {
        u32 color = *source;
        int r = color & 31;
        int g = ((color << 16) >> 21) & mask;
        int b = ((color << 16) >> 26) & mask;

        r = (bias + r * s->alpha) >> 4;
        g = (bias + g * s->alpha) >> 4;
        b = (bias + b * s->alpha) >> 4;
        s->colors[i] = (b << 10) | (g << 5) | r;
    }
}
