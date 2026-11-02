#include "scene_actor_availability.h"
#include "scene_actor_state.h"
#include "scene_actor_registry.h"
void SceneActorBehavior_UpdateColliders(struct SceneActorRegistryActor *);

s32 SceneActorMovement_RequestAvailableAction(struct SceneActorRegistryActor *actor)
{
    u32 unavailable;
    /* The unconditional first store requires a valid actor. */
    ((struct SceneActorRegistryResetView *)actor)->field304 = 0;
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
