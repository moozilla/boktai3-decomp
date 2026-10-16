#include "scene_actor_collision.h"
void SceneActorCollision_InitBodyWithFlags(struct SceneActorRegistryActor *actor, void *inputSize,
                                           void *inputOffset, void *inputCallback, void *owner,
                                           u32 flags)
{
    void *size = inputSize;
    void *offset = inputOffset;
    void *callback = inputCallback;
    struct ContactCollider *collider;
    {
        u32 colliderOffset = 0x134;
        collider = (void *)((u8 *)actor + colliderOffset);
    }
    Collider_Init(collider, (u16)actor->id, (u16)flags, 0, 16, size, offset);
    sub_0821FF7C(collider, SceneActorCollision_ReadBodyField3C(actor),
                 SceneActorCollision_ReadBodyReactionFlags(actor),
                 SceneActorCollision_ReadBodyField34(actor));
    Collider_SetPosition(collider, &actor->position, 0);
    Collider_SetCallback(collider, callback, owner);
    Collider_Refresh(collider);
}
