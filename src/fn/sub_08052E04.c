#include "stacked_actor.h"
#define StackedActor_RetractSegments sub_08052E04
void StackedActor_RetractSegments(struct StackedActor *actor)
{
    actor->sprite.flags &= ~1;
    if (actor->sprite.position.y < actor->world.y + 22)
        actor->sprite.position.y += 2;
    if (actor->height) {
        actor->height--;
        sub_08052418(actor);
    } else {
        actor->shadow.flags &= ~1;
        if (actor->removeAfterUse)
            sub_08052628(actor, sub_08052FE4);
        else
            sub_08052628(actor, sub_08052A40);
    }
}
