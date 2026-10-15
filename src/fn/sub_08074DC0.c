#include "scene_actor_collision.h"
#define SceneActorCollision_InitExternalAttack sub_08074DC0
void SceneActorCollision_InitExternalAttack(struct SceneActorRegistryActor *actor,
                                            struct ContactCollider *collider, void *size,
                                            void *offset, void *callback, u32 index, void *owner)
{
    {
        u32 flags = 0x2001;
        Collider_Init(collider, (u16)actor->id, flags, 0, 16, size, offset);
    }
    sub_0821FF58(collider, SceneActorCollision_ReadAttackField3C(actor, index),
                 SceneActorCollision_ReadAttackField40(actor, index), 0,
                 SceneActorCollision_ReadAttackReactionFlags(actor, index),
                 SceneActorCollision_ReadAttackField44(actor, index));
    Collider_SetPosition(collider, &actor->position, 0);
    Collider_SetCallback(collider, callback, owner);
}
