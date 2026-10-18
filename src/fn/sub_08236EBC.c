#include "global.h"
/* Tentative ParticleEffect/ParticleBurst family names (low confidence). */
#define ParticleBurst_AdvanceFrame sub_08236EBC
struct ParticleBurstSprite {
    u32 flags;
    u8 pad4[0x69];
    u8 frame;
};
void sub_08236E4C(struct ParticleBurstSprite *);
void sub_08236EBC(struct ParticleBurstSprite *p)
{
    if (p->frame <= 5)
        p->flags |= 0x400;
    else
        p->flags &= ~0x400;
    if (p->frame <= 15) {
        p->frame++;
        sub_08236E4C(p);
    }
}
