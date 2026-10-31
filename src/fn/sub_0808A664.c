#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_control_phase.h"

struct GhoulDiscContext {
    u8 unknown0[0x62C];
    struct ContactPosition targetPositions[2];
    u16 remaining, count;
    u8 unknown640[10];
    u16 recoveryDelay;
};

void Ghoul_UpdateAimedDiscThrow(struct SceneActorRegistryResetView *);

static inline void Ghoul_SetMotionFlags(struct SceneActorRegistryResetView *actor, u16 bits)
{
    SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&actor->flags2D4, bits);
}

/* Start a disc attack, then restore pursuit after its configured recovery delay. */
void sub_0808A664(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct GhoulDiscContext *context = actor->context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        Ghoul_SetMotionFlags(actor, 0x100);
        SceneActorChase_SetControl(actor, Ghoul_UpdateAimedDiscThrow, 0x23);
        context->remaining = context->count;
    }
    if (actor->field2B0) {
        SceneActorChase_ClearMotionFlagComponent(actor, 0x100);
        SceneActorChase_ClearWordFlags(&actor->flags2D0, 4);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12,
                                context->recoveryDelay);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        SceneActorChase_SetState(actor, 3, 0, 0);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      &context->targetPositions[0]))
            Callback_InvokeActor(actor, group->fallback);
    } else {
        actor->counter2AC++;
    }
}
