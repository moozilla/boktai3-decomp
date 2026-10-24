// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"

// foundry draft (worker best 96.2%); what differs is mostly register allocation and instruction order
#define SceneActorControl_EnterResetMode SceneActorPlacement_ResetActorState
void SceneActorPlacement_ResetActorState(struct SceneActorRegistryResetView *, const u16 *);
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        register u32 offset asm("r2") = 0x2D4;
        if (*(u16 *)((u8 *)actor + offset) & mask)
            return 1;
        mask = 0x20000;
        offset -= 4;
        {
            register u8 *q asm("r0");
            q = (u8 *)actor + offset;
            if (*(u32 *)q & mask)
                return 1;
        }
        if (actor->health <= 0)
            return 1;
    }
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
