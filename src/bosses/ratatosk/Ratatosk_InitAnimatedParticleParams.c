#include "global.h"
#include "bosses/ratatosk/ratatosk_particle.h"

void Memory_Clear(void *, u32);

void Ratatosk_InitAnimatedParticleParams(struct RatatoskParticleConfig *p)
{
    Memory_Clear(p, sizeof(*p));
    Ratatosk_SetAnimatedParticleUpdateCallback(p, Ratatosk_OnAnimatedParticleUpdate);
    p->appearanceFlags = 0;
    p->lifespan = 20;
    p->speed = 5;
    p->angle = 0;
    p->behavior = 0;
    p->fallAcceleration = 0x200;
}
