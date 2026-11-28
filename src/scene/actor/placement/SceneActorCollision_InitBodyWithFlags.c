#include "scene/collision/scene_actor_collision.h"
void SceneActorCollision_InitBodyWithFlags(struct ChaseActor *actor, void *size, void *offset,
                                           void *callback, void *owner, u32 flags)
{
    struct ContactCollider *collider = &actor->base.colliderA;
    Collider_Init(collider, (u16)actor->base.body.id, (u16)flags, 0, 16, size, offset);
    Collider_SetBodyProperties(collider, SceneActorCollision_ReadBodyField3C(actor),
                               SceneActorCollision_ReadBodyReactionFlags(actor),
                               SceneActorCollision_ReadBodyField34(actor));
    Collider_SetPosition(collider, &actor->base.body.position, 0);
    Collider_SetCallback(collider, callback, owner);
    Collider_Refresh(collider);
}
