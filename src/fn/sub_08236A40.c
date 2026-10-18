#include "global.h"
#define ParticleEffect_Destroy       sub_08236A40
#define ParticleEffect_RemoveSprites ActorBurst_RemovePrimarySprites
#define ParticleEffect_RemoveEffects RadialBurstEffect_RemoveParticles
#define gParticleEffectPendingCount  gUnk_02000590
/* Tentative family name; fields are named by their observed use. */
struct ParticleEffect {
    u8 pad[0x19];
    u8 pending;
    u16 stopFlags;
    u8 pad1C[0x94];
    u8 active;
    u8 suppressCleanup;
};
extern u16 gUnk_02000590;
void sub_08235DB0(struct ParticleEffect *);
void ActorBurst_RemovePrimarySprites(struct ParticleEffect *);
void ActorBurst_RemoveGlowSprite(struct ParticleEffect *);
void RadialBurstEffect_RemoveParticles(struct ParticleEffect *);
u32 sub_08236A40(struct ParticleEffect *p)
{
    if (p->active && !p->suppressCleanup && !p->stopFlags)
        sub_08235DB0(p);
    ActorBurst_RemovePrimarySprites(p);
    ActorBurst_RemoveGlowSprite(p);
    RadialBurstEffect_RemoveParticles(p);
    if (p->pending)
        gUnk_02000590--;
}
