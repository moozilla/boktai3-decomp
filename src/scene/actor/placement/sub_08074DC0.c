#include "scene/collision/scene_actor_collision.h"
void SceneActorCollision_InitExternalAttack(struct ChaseActor *actor,
                                            struct ContactCollider *collider, void *size,
                                            void *offset, void *callback, u32 index, void *owner)
{
    {
        u32 flags = 0x2001;
        Collider_Init(collider, (u16)actor->base.body.id, flags, 0, 16, size, offset);
    }
    Collider_SetAttackProperties(collider, SceneActorCollision_ReadAttackField3C(actor, index),
                                 SceneActorCollision_ReadAttackField40(actor, index), 0,
                                 SceneActorCollision_ReadAttackReactionFlags(actor, index),
                                 SceneActorCollision_ReadAttackField44(actor, index));
    Collider_SetPosition(collider, &actor->base.body.position, 0);
    Collider_SetCallback(collider, callback, owner);
}
