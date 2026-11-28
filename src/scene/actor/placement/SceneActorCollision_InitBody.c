#include "scene/collision/scene_actor_collision.h"
void SceneActorCollision_InitBody(struct ChaseActor *actor, void *size, void *offset,
                                  void *callback, void *owner)
{
    struct ContactCollider *collider = &actor->base.colliderA;
    Collider_Init(collider, (u16)actor->base.body.id, 0x5109, 0, 16, size, offset);
    Collider_SetBodyProperties(collider, SceneActorCollision_ReadBodyField3C(actor),
                               SceneActorCollision_ReadBodyReactionFlags(actor),
                               SceneActorCollision_ReadBodyField34(actor));
    Collider_SetPosition(collider, &actor->base.body.position, 0);
    Collider_SetCallback(collider, callback, owner);
    Collider_Refresh(collider);
}
