#include "scene/collision/scene_actor_collision.h"
void SceneActorCollision_InitAttack(struct ChaseActor *actor, void *size, void *offset,
                                    void *callback, u32 index, void *owner)
{
    struct ContactCollider *collider = &actor->base.colliderB;
    {
        u32 flags = 0x2401;
        Collider_Init(collider, (u16)actor->base.body.id, flags, 0, 16, size, offset);
    }
    Collider_SetAttackProperties(collider, SceneActorCollision_ReadAttackField3C(actor, index),
                                 SceneActorCollision_ReadAttackField40(actor, index), 0,
                                 SceneActorCollision_ReadAttackReactionFlags(actor, index),
                                 SceneActorCollision_ReadAttackField44(actor, index));
    Collider_SetPosition(collider, &actor->base.body.position, 0);
    Collider_SetCallback(collider, callback, owner);
}
