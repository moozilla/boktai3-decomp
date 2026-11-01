#include "global.h"
#include "scene_actor_reset.h"

void SceneActorBehavior_ApplyContactReaction(void *, void *, void *);

static inline u32 ActorFlag(struct SceneActorRegistryResetView *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}

static inline u8 IsUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor &&
        (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline u32 ContextFlag(void *context, u32 input)
{
    u32 mask = input;
    return *(u32 *)((u8 *)context + 0x62C) & mask;
}

void sub_080ED328(void *source, void *target, struct SceneActorRegistryResetView *actor)
{
    if (IsUnavailable(actor))
        return;
    if (Test(ContextFlag(actor->context, 1), ~0))
        return;
    SceneActorBehavior_ApplyContactReaction(source, target, actor);
}
