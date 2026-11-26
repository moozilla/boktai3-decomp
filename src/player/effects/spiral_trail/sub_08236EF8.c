#include "global.h"
/* Tentative ParticleEffect/ParticleBurst family names (low confidence). */
struct ParticleBurstSprite {
    u32 flags;
    u8 pad4[0x68];
    u8 active;
    u8 frame;
};
void RadialBurstEffect_BlendPaletteTowardWhite(struct ParticleBurstSprite *);
void SpiralTrail_RewindFrame(struct ParticleBurstSprite *p)
{
    if (p->frame <= 5)
        p->flags |= 0x400;
    else
        p->flags &= ~0x400;
    if (p->frame == 0) {
        p->flags |= 1;
        p->active = 0;
    } else {
        p->frame--;
        RadialBurstEffect_BlendPaletteTowardWhite(p);
    }
}
