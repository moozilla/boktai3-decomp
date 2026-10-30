#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"

/* The delayed response shares its first eight bytes with saved control. */
struct DogDelayedResponse {
    struct SceneActorChaseSavedControl saved;
    u8 state, phase, behavior, kind;
    s32 delay;
    void *control;
};

static inline void Dog_PrepareDelay(struct DogDelayedResponse *response, void *control,
                                      u32 behavior, s32 duration)
{
    response->delay = duration;
    response->control = control;
    response->state = 0;
    response->phase = 0;
    response->behavior = behavior;
}

static inline u32 Dog_RouteFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}

static inline u8 Dog_TakeStateEntry(struct SceneActorRegistryResetView *actor)
{
    return SceneActorControl_TakeFlag(&actor->field2B1);
}

/* Queue pursuit, a home route, or the short waiting response after the delay. */
void sub_080E3874(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group;
    u32 zero;
    Dog_TakeStateEntry(actor);
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
        return;
    if ((s32)actor->counter2AC <= 60)
        goto waiting;
    group = &actor->group;
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 2, actor->category);
    SceneActorChase_SetState(actor, 3, 1, zero);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        Dog_PrepareDelay((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_Arrive(actor);
    } else if (Dog_RouteFlags(actor, 0x10)) {
        Dog_PrepareDelay((void *)&group->saved, actor->entryTarget228, 7, 30);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position, group->destination)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    } else {
        Dog_PrepareDelay((void *)&group->saved, actor->entryTarget228, 2, 30);
        actor->field2C2 = 64;
    }
    return;
waiting:
    actor->counter2AC++;
}
