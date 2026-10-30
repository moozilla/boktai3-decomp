// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
u8 Skeleton_RerouteToTarget(void *);
u8 SceneActor_DispatchContextAction(void *, u32);
// confidence=low: after the action delay, restores a reaction or reroutes toward the player.
static inline u32 Motion(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u8 ContextFlags(u8 *context, u32 mask)
{
    if (*(u16 *)(context + 0x6D4) & mask)
        return 1;
    return 0;
}
static inline u16 ReactionFlagMask(u16 flag)
{
    return flag;
}
void Vorn_ResumeRouteAfterDelay(struct SceneActorRegistryResetView *actor, s32 frame)
{
    u8 *group = (u8 *)&actor->group.common;
    u8 *context = actor->context;
    if (actor->field2B2)
        actor->field2B2 = 0;
    {
        u32 mask = ReactionFlagMask(16);
        actor->flags2D6 = mask | actor->flags2D6;
    }
    if (frame <= 18)
        return;
    actor->field2B0 = 1;
    if (Motion(actor, 2)) {
        u32 mask;
        Chase_SetState(actor, 3, 0, 0);
        {
            u8 *base = group;
            asm("" : "+r"(base));
            Chase_SetCtl(actor, *(void **)(base + 0x208), 7);
        }
        mask = ~0x983;
        actor->flags2D6 = mask & actor->flags2D6;
    } else if (ContextFlags(context, 0x1D))
        SceneActor_DispatchContextAction(context, 1);
    else {
        actor->tracking.target = (u32)gPlayers[0] + 0x24;
        {
            u32 mask = 16;
            actor->flags2CC |= mask;
        }
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        if (!Skeleton_RerouteToTarget(actor) &&
            !Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position,
                            &actor->tracking.target->position)) {
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            return;
        }
        Chase_SetCtl(actor, actor->entryTarget228, 1);
        Chase_SetBehavior(actor, 7);
        actor->field2C2 = 32;
    }
}
