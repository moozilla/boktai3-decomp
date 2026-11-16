#include "scene_actor_reset.h"
u32 SceneActor_DispatchContextAction(struct ChaseActor *, u32);
void SceneActorAnimation_SelectDirectionalEntry(struct ChaseActor *, u32);
// confidence=low: enters directional animation three when the contact-ready flag is set.
u8 Vorn_BeginContactAnimation(struct ChaseActor *actor)
{
    if (actor->group.hitReactionLatch == 0 && (actor->base.field2E6 & 8)) {
        actor->base.hitLockTimer = 45;
        actor->group.hitReactionLatch = 65;
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
    }
    return SceneActor_DispatchContextAction(actor, 0);
}
