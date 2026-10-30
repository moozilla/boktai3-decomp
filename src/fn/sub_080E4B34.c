#define SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL 1
#define SCENE_ACTOR_CHASE_SCALAR_CELLS 1
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

static inline u8 Dog_TakeStateEntry(struct SceneActorRegistryResetView *actor)
{
    return SceneActorControl_TakeFlag(&actor->field2B1);
}

/* Re-arm the saved controller, then resume the delayed pursuit route. */
void Dog_BeginDelayedRouteRecovery(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 zero;
    if (Dog_TakeStateEntry(actor)) {
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
        SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x80);
        SceneActorChase_ClearMotionFlagComponent(actor, 0x200);
    }
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
        return;
    if ((s32)actor->counter2AC <= 20)
        goto waiting;
    SceneActorChase_ClearFlagComponent((struct SceneActorChaseFlags16 *)&group->common.gate.flags,
                                        6);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    Dog_PrepareDelay((void *)&group->saved, group->common.gate.control, 12,
                      group->common.gate.field41A);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_Arrive(actor);
    return;
waiting:
    actor->counter2AC++;
}
