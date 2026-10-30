#include "global.h"
#include "scene_actor_reset.h"
struct SceneActorRegistryActor;
void SceneActorCollision_InitBody(struct SceneActorRegistryActor *, void *, void *, void *, void *);
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void SceneActorBehavior_OnContactForward(void);
void Ghost_BeginContactReaction(void);
union LinkedColliderVector {
    struct ContactPackedVector packed;
    struct ContactPosition position;
};
void SceneActor_InitLinkedReactionColliders(struct SceneActorRegistryResetView *actor)
{
    union LinkedColliderVector size, offset;
    size.packed.x = 16;
    size.packed.y = 64;
    size.packed.z = 16;
    offset.packed.x = 0;
    offset.packed.y = 0;
    offset.packed.z = 0;
    SceneActorCollision_InitBody((void *)actor, &size, &offset, SceneActorBehavior_OnContactForward,
                                 actor);
    size.position.x = 24;
    size.position.y = 64;
    size.position.z = 24;
    offset.position.x = 0;
    offset.position.y = 0;
    offset.position.z = 0;
    {
        u32 flags = 0x2003;
        struct ContactCollider *collider = &actor->colliderB;
        Collider_Init(collider, actor->body.id, flags, 0, 16, &size, &offset);
        Collider_SetAttackProperties(&actor->colliderB, 0, 0, 0, 0, 0);
        Collider_SetPosition(&actor->colliderB, &actor->body.position, 0);
        Collider_SetCallback(&actor->colliderB, Ghost_BeginContactReaction, actor);
    }
}
