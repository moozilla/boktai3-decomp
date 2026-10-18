#define SpriteParticles_UpdateVariant ParticlePool_UpdateLinearParticle
#include "global.h"
struct SpriteParticle { u32 flags; u8 pad04[0x38]; };
struct SpriteParticleControl { u8 active; u8 pad01[5]; u8 lifetime; u8 variant; u8 pad08[0x34]; };
struct SpriteParticles { u8 pad00[0x74]; struct SpriteParticle entries[16]; u16 timer; };
void sub_0816385C(struct SpriteParticles *, struct SpriteParticle *);
void ParticlePool_UpdateLinearParticle(struct SpriteParticles *, struct SpriteParticle *);
void SpriteParticles_UpdateEntries(struct SpriteParticles *p)
{
    struct SpriteParticle *entry = p->entries;
    struct SpriteParticleControl *control = (void *)((u8 *)p + 0x9C);
    int i = 15;
    do {
        if (control->active) {
            if (--control->lifetime == 0) {
                entry->flags |= 1;
                control->active = 0;
            } else if (control->variant == 0) {
                sub_0816385C(p, entry);
            } else {
                ParticlePool_UpdateLinearParticle(p, entry);
            }
        }
        entry++;
        control++;
    } while (--i >= 0);
    p->timer++;
}
