#include "global.h"
/* Tentative ParticleEffect/ParticleBurst family names (low confidence). */
#define ParticleEffect_Update       sub_082369D8
#define gParticleEffectPendingCount gUnk_02000590
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
extern u16 gUnk_02000590;
void sub_08236644(struct ParticleEffect *);
void sub_08236878(struct ParticleEffect *);
void sub_08236428(struct ParticleEffect *);
void Task_Kill(void *);
u32 sub_082369D8(struct ParticleEffect *p)
{
    if (p->owner->state == 4) {
        p->stopped = 1;
        return 0;
    }
    if (p->pending) {
        gUnk_02000590--;
        p->pending = 0;
    }
    sub_08236644(p);
    sub_08236878(p);
    sub_08236428(p);
    if (!p->active12C && !p->activeB0 && !p->active13A)
        Task_Kill(p);
}
