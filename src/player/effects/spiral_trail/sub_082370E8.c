#include "global.h"
/* Tentative ParticleEffect/ParticleBurst family names (low confidence). */
#define SpiralTrail_RemoveSprite SpiralTrail_RemoveSprite
#define SpiralTrail_RemoveSlots  SpiralTrail_RemoveParticles
struct ParticleBurst {
    u8 pad[0x284];
    u8 mode;
};
void SpiralTrail_RemoveSprite(struct ParticleBurst *);
void SpiralTrail_RemoveParticles(struct ParticleBurst *);
u32 SpiralTrail_Destroy(struct ParticleBurst *p)
{
    SpiralTrail_RemoveSprite(p);
    if (p->mode == 1)
        SpiralTrail_RemoveParticles(p);
}
