
#ifdef NONMATCHING
// fuzzy: 97.4%
// foundry draft (worker best 96.2%); what differs is mostly register allocation and instruction order
#define SceneActorControl_RestartBehavior57 sub_080B5724
#define SceneActorControl_EnterResetMode    SceneActorPlacement_ResetActorState
#include "global.h"
#include "scene_actor_reset.h"
void SceneActorPlacement_ResetActorState(struct SceneActorRegistryResetView *, const u16 *);
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        u16 offset = 0x2D4;
        if (actor->flags2D4 & mask)
            return 1;
        mask = 0x20000;
        offset -= 4;
        if (actor->flags2D0 & mask)
            return 1;
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}
void sub_080B5724(struct SceneActorRegistryResetView *actor)
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
#else
INCLUDE_ASM("asm/nonmatching", sub_080B5724);
#endif
