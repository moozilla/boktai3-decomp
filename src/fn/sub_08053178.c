#include "stacked_actor.h"
void StackedActor_ConsumeTriggers(struct StackedActor *);
void sub_0824923C(struct StackedActor *, void (*)(struct StackedActor *));
void StackedActor_TickParticle(struct StackedActorParticle *);
s32 StackedActor_Tick(struct StackedActor *actor)
{
    s32 i;
    StackedActor_ConsumeTriggers(actor);
    sub_0824923C(actor, actor->update);
    for (i = 0; i < 8; i++)
        if (actor->particles[i].enabled)
            StackedActor_TickParticle(&actor->particles[i]);
    return 0;
}
