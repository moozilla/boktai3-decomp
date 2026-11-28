#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_state.h"
#include "scene/actor/scene_actor_registry.h"
void SceneActorBehavior_UpdateColliders(struct ChaseActor *);

s32 SceneActorMovement_RequestAvailableAction(struct ChaseActor *actor)
{

    /* The unconditional first store requires a valid actor. */
    actor->base.counter304 = 0;
    {
        u32 result;
        if (!SceneActor_ActionsBlocked(actor)) {
            SceneActorBehavior_UpdateColliders(actor);
            result = 1;
        } else {
            result = 0;
        }
        return result;
    }
}
