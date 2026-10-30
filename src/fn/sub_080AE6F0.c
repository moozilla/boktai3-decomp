#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

extern const u8 gUnk_0824E20C[];
u8 Vorn_HasActiveResponse(void *);
u8 SceneActor_DispatchContextAction(void *, u32);
void sub_080B3110(void *, u32, const u8 *, u32);
void Vorn_SendMessage(void *);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);

static inline u32 Vorn_ActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
static inline void Vorn_ClearTrackingFlags(struct SceneActorChaseTracking *tracking, u32 bits)
{
    u32 mask = ~bits;
    tracking->flags &= mask;
}
static inline void Vorn_SetTrackingFlags(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags = bits | tracking->flags;
}
static inline void Vorn_StartRoute(struct SceneActorRegistryResetView *actor,
                                   struct SceneActorChaseGroup *group)
{
    SceneBody_StartRoute(&actor->body.id, &group->homeRoute, group->homeRoute.routeId,
                         group->homeRoute.parameterB, group->homeRoute.pointId);
}
static inline void Vorn_ClearContextFlags(u16 *flags, u32 bits)
{
    u32 mask = ~bits;
    *flags &= mask;
}

void Vorn_ResumeReactionRoute(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(actor);
    struct SceneActorAnimationLink *context = actor->context;
    u32 *status;
    struct SceneActorChaseFlags16 *motion;
    u16 *contextFlags;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->body.direction = (((actor->body.direction + 128) & 255) >> 6) << 6;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 0);
    }
    status = &actor->flags2D0;
    {
        u32 value = *status;
        if (!(value & 16))
            return;
        value &= ~4;
        *status = value;
    }
    Vorn_SendMessage(actor);
    SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 0);
    motion = (struct SceneActorChaseFlags16 *)&actor->flags2D4;
    SceneActorChase_ClearFlagComponent(motion, 0x38);
    Vorn_StartRoute(actor, group);
    contextFlags = &context->flags;
    Vorn_ClearContextFlags(contextFlags, 10);
    if (Vorn_HasActiveResponse(context)) {
        Vorn_SetTrackingFlags(&actor->tracking, 8);
        SceneActorChase_SetActorFlags(actor, 0x80);
        Vorn_ClearContextFlags(&((struct SceneActorAnimationLink *)actor->context)->flags, 3);
        sub_080B3110(actor, 1, gUnk_0824E20C, 0x39F);
    } else if (!SceneActor_DispatchContextAction(context, 1)) {
        struct SceneActorRegistryControl *control;
        Vorn_ClearContextFlags(contextFlags, 0x1E);
        actor->field2C2 = 64;
        actor->group.common.gate.flags = 0;
        actor->group.common.gate.counter = 0;
        SceneActorChase_ClearFlagComponent(motion, 0x380);
        Vorn_ClearTrackingFlags(&actor->tracking, 0x18);
        control = &actor->group.common.control;
        if (Vorn_ActorFlags(actor, 0x200000)) {
            SceneActorChase_ClearWordFlags(status, 0x1000000);
            SceneActorChase_ClearFlagComponent(motion, 0x200);
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        SceneActorChase_ClearFlagComponent(motion, 0x400);
        actor->group.turn.turnsRemaining = -1;
        SceneActorChase_SetActorFlags(actor, 16);
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, group->field5E4, 15);
    }
    actor->field2C2 = 64;
}
