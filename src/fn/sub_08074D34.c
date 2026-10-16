#include "scene_actor_collision.h"
void SceneActorCollision_InitAttack(struct SceneActorRegistryActor *actor, void *inputSize,
                                    void *inputOffset, void *inputCallback, u32 inputIndex,
                                    void *owner)
{
    void *size = inputSize;
    void *offset = inputOffset;
    void *callback = inputCallback;
    u32 index = inputIndex;
    struct ContactCollider *collider;
    {
        u32 colliderOffset = 0x188;
        collider = (void *)((u8 *)actor + colliderOffset);
    }
    {
        u32 flags = 0x2401;
        Collider_Init(collider, (u16)actor->id, flags, 0, 16, size, offset);
    }
    sub_0821FF58(collider, SceneActorCollision_ReadAttackField3C(actor, index),
                 SceneActorCollision_ReadAttackField40(actor, index), 0,
                 SceneActorCollision_ReadAttackReactionFlags(actor, index),
                 SceneActorCollision_ReadAttackField44(actor, index));
    Collider_SetPosition(collider, &actor->position, 0);
    Collider_SetCallback(collider, callback, owner);
}
