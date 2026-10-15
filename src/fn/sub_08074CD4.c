#include "scene_actor_collision.h"
void SceneActorCollision_InitExternalBody(struct SceneActorRegistryActor *actor,
                                          struct ContactCollider *collider, void *size,
                                          void *offset, void *callback, void *owner, u32 flags)
{
    Collider_Init(collider, (u16)actor->id, (u16)flags, 0, 16, size, offset);
    sub_0821FF7C(collider, SceneActorCollision_ReadBodyField3C(actor),
                 SceneActorCollision_ReadBodyReactionFlags(actor),
                 SceneActorCollision_ReadBodyField34(actor));
    Collider_SetPosition(collider, &actor->position, 0);
    Collider_SetCallback(collider, callback, owner);
    Collider_Refresh(collider);
}
