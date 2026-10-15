#include "stacked_actor.h"
#define StackedActor_TickParticle sub_08052130
void *sub_08215184(u32);
void sub_08217EEC(struct StackedActorParticle *, void *, u32);
static inline s32 damp(s32 v)
{
    s32 scaled = v * 15;
    if (scaled >= 0)
        return scaled >> 4;
    return -((-scaled) >> 4);
}
void StackedActor_TickParticle(struct StackedActorParticle *particle)
{
    particle->life--;
    if (particle->life == 0) {
        particle->flags |= 1;
        particle->enabled = 0;
    } else {
        particle->position.x += particle->velocityX;
        particle->position.y += particle->velocityY;
        particle->position.z += particle->velocityZ;
        sub_08217EEC(particle, sub_08215184(0x1C1E), ((particle->life >> 2) & 1) + 2);
        particle->velocityX = damp(particle->velocityX);
        particle->velocityY = damp(particle->velocityY);
        particle->velocityZ = damp(particle->velocityZ);
    }
}
