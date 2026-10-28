#include "global.h"
#include "ram.h"
/* Tentative ParticleEffect/ParticleBurst family names (low confidence). */
struct ParticleEffect {
    u8 pad0[0x19];
    u8 pending;
    u8 pad1A;
    u8 stopped;
    struct ParticleEffectOwner *owner;
    u8 pad20[0x90];
    u8 activeB0;
    u8 padB1[0x7B];
    u8 active12C;
    u8 pad12D[0xD];
    u8 active13A;
};
struct ParticleEffectOwner {
    u8 pad[0x1C];
    u32 state;
};
void PaletteTransition_UpdateOwnerFade(struct ParticleEffect *);
void PlayerEffect_UpdatePhaseCallback(struct ParticleEffect *);
void CallbackGroup_UpdateActive(struct ParticleEffect *);
void Task_Kill(void *);
u32 ParticleEffect_Update(struct ParticleEffect *p)
{
    if (p->owner->state == 4) {
        p->stopped = 1;
        return 0;
    }
    if (p->pending) {
        gPlayerEffectPendingCount--;
        p->pending = 0;
    }
    PaletteTransition_UpdateOwnerFade(p);
    PlayerEffect_UpdatePhaseCallback(p);
    CallbackGroup_UpdateActive(p);
    if (!p->active12C && !p->activeB0 && !p->active13A)
        Task_Kill(p);
}
