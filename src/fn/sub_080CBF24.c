#include "global.h"
#include "scene_actor_control_phase.h"
#define SCENE_ACTOR_CHASE_WIDE_STATE           1
#define SCENE_ACTOR_CHASE_SCALAR_CELLS         1
#define SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL 1
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

/* The delayed response shares its first eight bytes with saved control. */
struct GolemDelayedResponse {
    struct SceneActorChaseSavedControl saved;
    u8 state, phase, behavior, kind;
    s32 delay;
    void *control;
};

static inline void Golem_PrepareDelay(struct GolemDelayedResponse *response, void *control,
                                      u32 behavior, s32 duration)
{
    response->delay = duration;
    response->control = control;
    response->state = 0;
    response->phase = 0;
    response->behavior = behavior;
}

static inline u8 Golem_TakeStateEntry(struct SceneActorRegistryResetView *actor)
{
    return SceneActorControl_TakeFlag(&actor->field2B1);
}

/* Queue pursuit or a return-home route after the recovery delay. */
void Golem_ResumeDelayedRoute(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 zero;
    Golem_TakeStateEntry(actor);
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
        return;
    if ((s32)actor->counter2AC <= 60)
        goto waiting;
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 2, actor->category);
    SceneActorChase_SetState(actor, 3, 1, zero);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        Golem_PrepareDelay((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_Arrive(actor);
    } else {
        SceneActorChase_SetActorFlags(actor, 0x10);
        Golem_PrepareDelay((void *)&group->saved, actor->entryTarget228, 7, 30);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    }
    return;
waiting:
    actor->counter2AC++;
}
