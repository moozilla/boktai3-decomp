#include "engine/shared/global.h"
#include "engine/shared/scene_actor_terrain_reaction.h"
s32 Callback_InvokeActor(void *, void *);

// confidence=low: scene actor state tick: clears the entry flag, runs the update callback and counts frames while it reports idle.
void sub_08137240(struct ChaseActor *actor)
{
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        actor->base.counter2AC++;
}
