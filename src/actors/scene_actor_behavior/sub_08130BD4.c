#include "engine/shared/scene_actor_terrain_reaction.h"
#include "engine/shared/scene_actor_status.h"
#define SceneActorRoute_GetStepDuration Route_GetCurrentPointNodeId
#include "engine/shared/global.h"
#include "engine/shared/scene_actor_control_phase.h"
u16 SceneActorRoute_GetStepDuration(void *);
void SceneActorRoute_AdvanceActorPoint(void *, void *);
void SceneActorRoute_UpdateTimedPhase(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    void *route = actor->base.body.currentRoute;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorTerrain_InstallControl(actor, group->routePhaseCallback, 5);
        group->field5C6 = SceneActorRoute_GetStepDuration(route);
    }
    if ((s16)group->field5C6 > 0)
        group->field5C6--;
    if (!(u8)Callback_InvokeActor(actor, actor->base.targetQuery) && actor->base.stepDone &&
        ActorStatus_BehaviorFlags(actor, 0x400000))
        SceneActorRoute_AdvanceActorPoint(actor, route);
}
