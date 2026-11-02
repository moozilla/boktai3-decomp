#include "global.h"
#include "scene_actor_reaction.h"

// foundry draft (worker best 94.9%); what differs is mostly register allocation and instruction order
struct SceneActorApproachActionContext {
    u8 unknown0[0x6F2];
    u16 timer;
    u8 unknown6F4[6];
    u16 duration;
    u8 unknown6FC[0x14];
    struct ContactPosition destination;
};
void sub_080BAAF4(void);
void Vorn_SpawnHazard(struct SceneActorRegistryResetView *, s32);
void Skeleton_SelectDistanceReaction(struct SceneActorRegistryResetView *);
static inline u8 TakeReady(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void ClearFlags(u16 *p, u32 mask)
{
    *p &= mask;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *fn)
{
    u8 code = 0x2C;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = code;
    actor->target = fn;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = 0;
}
void SceneActorApproach_CaptureTargetAndRunAction(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorApproachActionContext *context = actor->context;
    if (TakeReady(actor)) {
        SceneActorReaction_EnableMotionFlags(0x100, &actor->flags2D4);
        SetControl(actor, sub_080BAAF4);
        context->destination = actor->tracking.target->position;
        context->timer = context->duration;
        Vorn_SpawnHazard(actor, 2);
    }
    if (actor->field2B0) {
        ClearFlags(&actor->flags2D4, ~0x100);
        Skeleton_SelectDistanceReaction(actor);
    } else
        actor->counter2AC++;
}
