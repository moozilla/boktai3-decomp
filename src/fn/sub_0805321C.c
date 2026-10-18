#include "stacked_actor.h"
void *AnimData_FindEntry(u32);
void sub_08217DE0(struct StackedActorParticle *, void *, u32);
void Effect_SetOrigin(struct StackedActorParticle *, s32, s32);
void Effect_SetPalette(struct StackedActorParticle *, u32);
void SpriteDrawable_SelectTileFrame(struct StackedActorParticle *, void *, u32);
void StackedActor_InitParticles(struct StackedActor *actor)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        struct StackedActorParticle *particle = &actor->particles[i];
        sub_08217DE0(particle, AnimData_FindEntry(0x1C1E), 1);
        Effect_SetOrigin(particle, -4, -4);
        Effect_SetPalette(particle, 1);
        SpriteDrawable_SelectTileFrame(particle, AnimData_FindEntry(0x1C1E), 2);
        particle->renderMode = 1;
        particle->priority = 20;
        particle->position = actor->world;
        actor->particles[i].enabled = 0;
    }
}
