#define SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL 1
#define SCENE_ACTOR_CHASE_SCALAR_CELLS         1
#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_control_phase.h"

/* Re-arm the saved controller, then resume the delayed pursuit route. */
void SceneActorReaction_UpdateRouteReturn(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 zero;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
        SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x80);
        SceneActorChase_ClearMotionFlagComponent(actor, 0x200);
    }
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
        return;
    if ((s32)actor->counter2AC <= 60)
        goto waiting;
    SceneActorChase_ClearFlagComponent((struct SceneActorChaseFlags16 *)&group->common.gate.flags,
                                       6);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12,
                            group->common.gate.field41A);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_Arrive(actor);
    return;
waiting:
    actor->counter2AC++;
}
