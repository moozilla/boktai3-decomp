#include "global.h"
/* Tentative ParticleEffect/ParticleBurst family names (low confidence). */
struct ParticleBurstSprite {
    u32 flags;
    u8 pad4[0x69];
    u8 frame;
};
void RadialBurstEffect_BlendSpritePalette_08236E4C(struct ParticleBurstSprite *);
void ParticleBurst_AdvanceFrame(struct ParticleBurstSprite *p)
{
    if (p->frame <= 5)
        p->flags |= 0x400;
    else
        p->flags &= ~0x400;
    if (p->frame <= 15) {
        p->frame++;
        RadialBurstEffect_BlendSpritePalette_08236E4C(p);
    }
}
