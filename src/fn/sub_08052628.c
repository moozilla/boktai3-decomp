#include "stacked_actor.h"
void StackedActor_SetUpdate(struct StackedActor *actor, void (*update)(struct StackedActor *))
{
    actor->update = update;
    actor->frame = 0;
}
