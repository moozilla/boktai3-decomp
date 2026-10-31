#define SCENE_ACTOR_CHASE_SCALAR_CELLS         1
#define SCENE_ACTOR_CHASE_ROUTE_COUNT_ACCESSOR 1
#define SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL 1
#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_delayed_response.h"

struct MummyRouteContext {
    u8 unknown0[0x6D2];
    u16 pursuitDelay;
};
struct MummyRouteTarget {
    struct SceneActorChaseTarget target;
    u8 unknown14[8];
    struct SceneActorChasePlayerCell *cell;
};

u8 Mummy_RequestStraightRoute(struct SceneActorRegistryResetView *);
void SceneActorMovement_ResolveRoutePosition(struct SceneActorRegistryResetView *);

void Mummy_ResumeDelayedRoute(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct MummyRouteContext *body = actor->context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
        SceneActorChase_ClearMotionFlagComponent(actor, 0x200);
        SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x80);
        group->common.control.timer = 0;
    }
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if ((s32)actor->counter2AC > 60) {
        SceneActorChase_ClearFlagComponent(&group->common.gate.flags, 6);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_SetState(actor, 3, 1, 0);
        if (body->pursuitDelay &&
            actor->terrainCell ==
                ((struct MummyRouteTarget *)actor->tracking.target)->cell->index) {
            if (Mummy_RequestStraightRoute(actor))
                return;
            SceneActorChase_ChooseRoute(actor);
            SceneActorMovement_ResolveRoutePosition(actor);
            SceneActorChase_SetFlagComponent(&group->common.gate.flags, 4);
        } else
            SceneActorChase_Arrive(actor);
    } else
        actor->counter2AC++;
}
