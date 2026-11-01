#include "scene_actor_chase.h"
#include "debug.h"
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
struct VornRouteGroup {
    u8 pad[0x1FC];
    void *mesh;
    struct ContactPosition *destination;
    u8 pad204[4];
    void *control;
    u8 pad20C[0x18];
    void *failure;
};
static inline u8 TakePrepared(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2) {
        actor->field2B2 = 0;
        return 1;
    }
    return 0;
}
void Vorn_ResumeRouteAfterDelay(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct VornRouteGroup *group = (void *)&actor->group.common;
    u8 *context = actor->context;
    if (TakePrepared(actor)) {
        DEBUG_PRINT();
    }
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
        Chase_SetCtl(actor, group->control, 7);
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
        group->destination->pad = 0;
        if (!Skeleton_RerouteToTarget(actor) &&
            !Chase_TryRoute(actor, group->mesh, &actor->body.position,
                            &actor->tracking.target->position)) {
            Callback_InvokeActor(actor, group->failure);
            return;
        }
        Chase_SetCtl(actor, actor->entryTarget228, 1);
        Chase_SetBehavior(actor, 7);
        actor->field2C2 = 32;
    }
}
