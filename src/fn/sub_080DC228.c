#define SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL 1
#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

void SceneActor_UpdateDirectionalAnimation(void);
struct MummyPursuitContext {
    u8 unknown0[0x6F2];
    struct SceneActorChaseFlags16 flags;
};

void Mummy_ResumePlayerPursuit(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct MummyPursuitContext *body = actor->context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorChase_SetControl(actor, SceneActor_UpdateDirectionalAnimation, 3);
    }
    if (actor->field2B0) {
        SceneActorChase_ClearFlagComponent(&body->flags, 0x800);
        actor->tracking.target = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
        if (SceneActorChase_Arrive(actor)) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
            SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
            SceneActorChase_SetState(actor, 3, 1, 0);
        }
    }
}
