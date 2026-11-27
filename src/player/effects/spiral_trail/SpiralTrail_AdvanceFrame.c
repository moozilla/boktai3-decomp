#include "global.h"
#include "player/effects/spiral_trail_sprite.h"
void RadialBurstEffect_BlendPaletteTowardWhite(struct SpiralTrailSprite *);
void SpiralTrail_AdvanceFrame(struct SpiralTrailSprite *p)
{
    if (p->alpha <= 5)
        p->drawable.flags |= 0x400;
    else
        p->drawable.flags &= ~0x400;
    if (p->alpha <= 15) {
        p->alpha++;
        RadialBurstEffect_BlendPaletteTowardWhite(p);
    }
}
