#include "stacked_actor.h"
#define StackedActor_InitParticles sub_0805321C
void *AnimData_FindEntry(u32);
void sub_08217DE0(struct StackedActorParticle *, void *, u32);
void sub_08217EC4(struct StackedActorParticle *, s32, s32);
void sub_08217ECC(struct StackedActorParticle *, u32);
void sub_08217EEC(struct StackedActorParticle *, void *, u32);
void StackedActor_InitParticles(struct StackedActor *actor)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        struct StackedActorParticle *particle = &actor->particles[i];
        sub_08217DE0(particle, AnimData_FindEntry(0x1C1E), 1);
        sub_08217EC4(particle, -4, -4);
        sub_08217ECC(particle, 1);
        sub_08217EEC(particle, AnimData_FindEntry(0x1C1E), 2);
        particle->renderMode = 1;
        particle->priority = 20;
        particle->position = actor->world;
        actor->particles[i].enabled = 0;
    }
}
