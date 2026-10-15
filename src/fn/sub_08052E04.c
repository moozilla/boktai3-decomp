#include "stacked_actor.h"
#define StackedActor_RetractSegments sub_08052E04
void StackedActor_RetractSegments(struct StackedActor *actor)
{
    actor->sprite.flags &= ~1;
    if (actor->sprite.position.y < actor->world.y + 22)
        actor->sprite.position.y += 2;
    if (actor->height) {
        actor->height--;
        StackedActor_UpdateSegments(actor);
    } else {
        actor->shadow.flags &= ~1;
        if (actor->removeAfterUse)
            StackedActor_SetUpdate(actor, StackedActor_Disappear);
        else
            StackedActor_SetUpdate(actor, StackedActor_TickIdle);
    }
}
