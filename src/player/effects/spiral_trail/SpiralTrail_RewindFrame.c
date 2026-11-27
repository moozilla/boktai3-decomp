#include "global.h"
#include "player/effects/spiral_trail_sprite.h"
/* Tentative ParticleEffect/ParticleBurst family names (low confidence). */
void RadialBurstEffect_BlendPaletteTowardWhite(struct SpiralTrailSprite *);
void SpiralTrail_RewindFrame(struct SpiralTrailSprite *p)
{
    if (p->alpha <= 5)
        p->flags |= 0x400;
    else
        p->flags &= ~0x400;
    if (p->alpha == 0) {
        p->flags |= 1;
        p->b6c = 0;
    } else {
        p->alpha--;
        RadialBurstEffect_BlendPaletteTowardWhite(p);
    }
}
