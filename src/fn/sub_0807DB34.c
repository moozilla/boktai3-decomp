#include "scene_actor_status.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_reaction.h"

static inline void SceneActorRecovery_ClearGate(struct SceneActorControlGate *gate, u16 bits)
{
    u32 mask = ~bits;
    gate->flags &= mask;
}
void SceneActorRecovery_ResumePursuit(struct SceneActorRegistryResetView *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &actor->group;

    ActorStatus_ReleaseContact(actor);
    actor->field2C2 = 64;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    SceneActorChase_SetState(actor, 3, 1, 0);
    if (ActorStatus_MotionFlags(actor, 0x80)) {
        if (SceneActorChase_Arrive(object)) {
            SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 60);
            SceneActorRecovery_ClearGate(&group->common.gate, 4);
        }
    } else {
        group->destination->pad = 1;
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      group->destination)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
        SceneActorDelay_Prepare((void *)&group->saved, actor->entryTarget228, 7, 60);
        {
            struct SceneActorRegistryControl *control = &group->common.control;
            if (ActorStatus_BehaviorFlags(actor, 0x200000))
                ActorStatus_ClearInteraction(actor, control);
        }
    }
}
