#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_placement.h"

// confidence=low: resumes the actor drawable and shadow, then restores its reset control.

void SceneActorTerrain_ResumeDrawable(void *);
void ActorShadow_Enable(void *);

static inline void ClearCollider(struct ContactCollider *collider)
{
    collider->flags = 0xFFFB & collider->flags;
}
void SceneActorPlacement_ResumeResetControl(struct RoomActor *actor)
{
    u32 bit = 1;
    if (actor->behaviorFlags & bit) {
        SceneActorTerrain_ResumeDrawable(actor);
        if (actor->shadow[0])
            ActorShadow_Enable(actor->shadow);
        ClearCollider(&actor->colliderA);
        SceneActorRegistry_ResetRouteAndColliders((void *)actor);
        SceneActorTerrain_EnterState((struct ChaseActor *)actor, 3, 0, 0);
        {
            const void *control = actor->onHidden;
            SceneActorTerrain_InstallControl((struct ChaseActor *)actor, control, 12);
        }
    }
}
