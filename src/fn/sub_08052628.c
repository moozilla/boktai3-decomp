#include "stacked_actor.h"
#define StackedActor_SetUpdate sub_08052628
void StackedActor_SetUpdate(struct StackedActor *actor, void (*update)(struct StackedActor *))
{
    actor->update = update;
    actor->frame = 0;
}
