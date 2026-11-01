#include "scene_actor_control_phase.h"
#include "scene_actor_placement.h"
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorView *);
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{
    return SceneActorPlacement_IsUnavailable((struct SceneActorPlacementView *)actor);
}
void TentacleActor_EnterHitReaction(struct SceneActorRegistryResetView *);
void TentacleBody_OnBodyContact(struct SceneActorBehaviorContactView *source,
                                struct SceneActorBehaviorContactView *destination,
                                struct SceneActorRegistryResetView *actor)
{
    if (Unavailable(actor))
        return;
    SceneActorBehavior_ApplyContactReaction(source, destination, (void *)actor);
    TentacleActor_EnterHitReaction(actor);
}
