#include "global.h"
#include "engine/ram.h"
#define ActionFlash_RemoveSprites ActionFlashBurst_RemovePrimarySprites
#define ActionFlash_RemoveEffects RadialBurstEffect_RemoveParticles
/* Tentative family name; fields are named by their observed use. */
struct ParticleEffect {
    u8 pad[0x19];
    u8 pending;
    u16 stopFlags;
    u8 pad1C[0x94];
    u8 active;
    u8 suppressCleanup;
};
void sub_08235DB0(struct ParticleEffect *);
void ActionFlashBurst_RemovePrimarySprites(struct ParticleEffect *);
void ActionFlashBurst_RemoveGlowSprite(struct ParticleEffect *);
void RadialBurstEffect_RemoveParticles(struct ParticleEffect *);
u32 ActionFlash_Destroy(struct ParticleEffect *p)
{
    if (p->active && !p->suppressCleanup && !p->stopFlags)
        sub_08235DB0(p);
    ActionFlashBurst_RemovePrimarySprites(p);
    ActionFlashBurst_RemoveGlowSprite(p);
    RadialBurstEffect_RemoveParticles(p);
    if (p->pending)
        gPlayerEffectPendingCount--;
}
