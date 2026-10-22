#include "global.h"
#include "scene_actor_registry.h"
void SceneActorCollision_InitExternalBody(struct SceneActorRegistryActor *,
                                          struct ContactCollider *, void *, void *, void *, void *,
                                          u32);
void SceneActorCollision_InitBody(struct SceneActorRegistryActor *, void *, void *, void *, void *);
void SceneActorCollision_InitAttack(struct SceneActorRegistryActor *, void *, void *, void *, u32,
                                    void *);
void SceneActorCollision_InitExternalAttack(struct SceneActorRegistryActor *,
                                            struct ContactCollider *, void *, void *, void *, u32,
                                            void *);
void SceneActorReaction_TryStartContact(void);
void SceneActor_TryForward(void);
void Skeleton_OnAttackCollision(void);
void Skeleton_OnContactEvent(void);
void SceneActorControl_InitBehaviorColliders(struct SceneActorRegistryActor *actor)
{
    u8 *context = actor->context;
    union SceneActorRegistryPackedPosition bodySize, bodyOffset;
    union SceneActorRegistryPackedPosition innerSize, innerOffset;
    union SceneActorRegistryPackedPosition attackSize, attackOffset;
    union SceneActorRegistryPackedPosition extraSize, extraOffset;
    bodySize.packed.x = 0x80;
    bodySize.packed.y = 0x80;
    bodySize.packed.z = 0x80;
    *(u32 *)&bodyOffset = 0x800000;
    *(u32 *)((u8 *)&bodyOffset + 4) &= 0xFFFF0000;
    {
        u32 flags = 0x410B;
        SceneActorCollision_InitExternalBody(actor, (void *)(context + 0x624), &bodySize,
                                             &bodyOffset, SceneActorReaction_TryStartContact, actor,
                                             flags);
    }
    innerSize.packed.x = 0x80;
    innerSize.packed.y = 0x80;
    innerSize.packed.z = 0x80;
    *(u32 *)&innerOffset = 0x800000;
    *(u32 *)((u8 *)&innerOffset + 4) &= 0xFFFF0000;
    SceneActorCollision_InitBody(actor, &innerSize, &innerOffset, SceneActor_TryForward, actor);
    attackSize.packed.x = 0x40;
    attackSize.packed.y = 0x64;
    attackSize.packed.z = 0x40;
    *(u32 *)&attackOffset = 0x800000;
    *(u32 *)((u8 *)&attackOffset + 4) &= 0xFFFF0000;
    SceneActorCollision_InitAttack(actor, &attackSize, &attackOffset, Skeleton_OnAttackCollision, 0,
                                   actor);
    *(u32 *)&extraSize = 0x640000;
    *(u32 *)((u8 *)&extraSize + 4) &= 0xFFFF0000;
    *(u32 *)&extraOffset = 0x800000;
    *(u32 *)((u8 *)&extraOffset + 4) &= 0xFFFF0000;
    SceneActorCollision_InitExternalAttack(actor, (void *)(context + 0x678), &extraSize,
                                           &extraOffset, Skeleton_OnContactEvent, 1, actor);
}
