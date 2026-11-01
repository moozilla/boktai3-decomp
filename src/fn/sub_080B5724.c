#include "global.h"
#include "scene_actor_reset.h"

// foundry draft (worker best 96.2%); what differs is mostly register allocation and instruction order
#define SceneActorControl_EnterResetMode SceneActorPlacement_ResetActorState
void SceneActorPlacement_ResetActorState(struct SceneActorRegistryResetView *, const u16 *);
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{
    if (!actor)
        goto zero;
    {
        u32 mask;
        u32 offset;
        u32 *field;
        u32 value;
        mask = 0x40;
        offset = 0x2d4;
        field = (void *)((u8 *)actor + offset);
        value = *(u16 *)field;
        value &= mask;
        if (value)
            goto one;
        mask = 0x20000;
        offset -= 4;
        field = (void *)((u8 *)actor + offset);
        value = *field;
        value &= mask;
        if (value)
            goto one;
    }
    if (actor->health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
void SceneActorControl_RestartBehavior57(struct SceneActorRegistryResetView *actor)
{
    u16 mode;
    if (Unavailable(actor))
        return;
    {
        u32 mask = 0x800;
        if (actor->flags2D4 & mask)
            return;
    }
    if (actor->field2B5 != 0x39)
        return;
    {
        u32 offset = 0x130;
        u32 *flags = (void *)((u8 *)actor + offset);
        *flags &= ~2;
    }
    {
        u32 offset = 0x2CC;
        u32 *flags = (void *)((u8 *)actor + offset);
        u32 mask = ~3;
        *flags &= mask;
    }
    SceneActorRegistry_ResetRoute((void *)actor);
    mode = 1;
    SceneActorPlacement_ResetActorState(actor, &mode);
}
