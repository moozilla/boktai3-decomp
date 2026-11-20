#include "global.h"
#include "scene_actor_placement.h"

void SceneActorControl_RestoreInactiveDefaults(struct ChaseActor *actor)
{
    u32 one;
    u32 flags;
    if (!SceneActorPlacement_IsUnavailable((struct RoomActor *)actor)) {
        one = 1;
        flags = actor->base.behaviorFlags;
        one &= flags;
        if (!one && (flags & 2)) {
            SceneActorRegistry_RestoreActorDefaults(actor);
            actor->base.colliderA.responseValue = one;
        }
    }
}
