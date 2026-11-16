#include "scene_actor_availability.h"
#include "scene_actor_state.h"
#include "scene_actor_registry.h"
void SceneActorBehavior_UpdateColliders(struct ChaseActor *);

s32 SceneActorMovement_RequestAvailableAction(struct ChaseActor *actor)
{
    u32 unavailable;
    /* The unconditional first store requires a valid actor. */
    ((struct ChaseActor *)actor)->base.counter304 = 0;
    unavailable = SceneActor_ActionsBlocked(actor);
    {
        u32 result;
        if (!unavailable) {
            SceneActorBehavior_UpdateColliders(actor);
            result = 1;
        } else {
            result = 0;
        }
        return result;
    }
}
