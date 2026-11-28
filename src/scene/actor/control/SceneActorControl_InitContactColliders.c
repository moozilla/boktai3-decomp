#include "scene/actor/scene_actor_control_phase.h"
#include "global.h"
#include "scene/collision/scene_actor_collision.h"
void SceneActorCollision_InitBody(struct ChaseActor *, void *, void *, void *, void *);
void SceneActorCollision_InitExternalAttackWithField34(struct ChaseActor *,
                                                       struct ContactCollider *, void *, void *,
                                                       void *, u32, u32, void *);
void SceneActorControl_ApplyContactPowerFloor(struct SceneActorBehaviorContactView *,
                                              struct SceneActorBehaviorContactView *,
                                              struct ChaseActor *);
void SceneActorControl_ApplyRecoveryContact(struct SceneActorBehaviorContactView *,
                                            struct SceneActorBehaviorContactView *,
                                            struct ChaseActor *);

void SceneActorControl_InitContactColliders(struct ChaseActor *actor)
{
    struct ContactPackedVector size, offset, attackSize, attackOffset;
    size.x = 0x20;
    size.y = 0x52;
    size.z = 0x20;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    SceneActorCollision_InitBody(actor, &size, &offset,
                                 (void *)SceneActorControl_ApplyContactPowerFloor, actor);
    attackSize.x = 0x20;
    attackSize.y = 0x2a;
    attackSize.z = 0x20;
    attackOffset.x = 0;
    attackOffset.y = 0;
    attackOffset.z = 0;
    SceneActorCollision_InitExternalAttackWithField34(
        actor, &actor->base.colliderB, &attackSize, &attackOffset,
        (void *)SceneActorControl_ApplyRecoveryContact, 0, 0x200000, actor);
}
