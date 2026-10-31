#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_control_phase.h"

void sub_080D49DC(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 zero;
    SceneActorControl_TakeStateEntry(actor);
    zero = (u8)Callback_InvokeActor(actor, actor->contactQuery.opaque);
    if (zero)
        return;
    if ((s32)actor->counter2AC <= 60)
        goto waiting;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
    SceneActorChase_SetState(actor, 3, 1, zero);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetActorFlags(actor, 0x80);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetFlagComponent(&group->common.gate.flags, 4);
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
    } else {
        SceneActorChase_SetActorFlags(actor, 0x10);
        SceneActorDelay_Prepare((void *)&group->saved, actor->entryTarget228, 7, 30);
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
