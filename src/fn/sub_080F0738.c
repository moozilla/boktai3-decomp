#define SceneActor_InitCollision5005 sub_080F0738
#include "global.h"
#include "scene_actor_registry.h"
void sub_080ED328(void);
void Ivy_OnSecondaryContact(void);
void SceneActorCollision_InitBodyWithFlags(struct SceneActorRegistryActor *, void *, void *,
                                           void (*)(void), struct SceneActorRegistryActor *, u32);
void SceneActorCollision_InitAttack(struct SceneActorRegistryActor *, void *, void *,
                                    void (*)(void), u32, struct SceneActorRegistryActor *);
void sub_080F0738(struct SceneActorRegistryActor *actor)
{
    union SceneActorRegistryPackedPosition bodySize, bodyOffset, attackSize, attackOffset;
    bodySize.packed.x = 0x40;
    bodySize.packed.y = 0x7f;
    bodySize.packed.z = 0x40;
    *(u32 *)&bodyOffset = 0x800000;
    *(u32 *)((u8 *)&bodyOffset + 4) &= 0xFFFF0000;
    SceneActorCollision_InitBodyWithFlags(actor, &bodySize, &bodyOffset, sub_080ED328, actor,
                                          0x5005);
    *(u32 *)&attackSize = 0;
    *(u32 *)((u8 *)&attackSize + 4) &= 0xFFFF0000;
    *(u32 *)&attackOffset = 0;
    *(u32 *)((u8 *)&attackOffset + 4) &= 0xFFFF0000;
    SceneActorCollision_InitAttack(actor, &attackSize, &attackOffset, Ivy_OnSecondaryContact, 1, actor);
}
