#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_control_phase.h"

static inline u32 Vorn_RouteFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}

/* Queue pursuit, a home route, or the short waiting response after the delay. */
void sub_080AF2BC(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 zero;
    SceneActorControl_TakeStateEntry(actor);
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
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_Arrive(actor);
    } else if (Vorn_RouteFlags(actor, 0x10)) {
        SceneActorDelay_Prepare((void *)&group->saved, actor->entryTarget228, 7, 30);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    } else {
        SceneActorDelay_Prepare((void *)&group->saved, actor->entryTarget228, 2, 30);
        actor->field2C2 = 64;
    }
    return;
waiting:
    actor->counter2AC++;
}
