#include "stacked_actor.h"
#define StackedActor_Tick sub_08053178
void sub_080530EC(struct StackedActor *);
void sub_0824923C(struct StackedActor *, void (*)(struct StackedActor *));
void sub_08052130(struct StackedActorParticle *);
s32 StackedActor_Tick(struct StackedActor *actor)
{
    s32 i;
    sub_080530EC(actor);
    sub_0824923C(actor, actor->update);
    for (i = 0; i < 8; i++)
        if (actor->particles[i].enabled)
            sub_08052130(&actor->particles[i]);
    return 0;
}
