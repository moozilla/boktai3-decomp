#include "scene_actor_terrain_reaction.h"
#include "scene_actor_status.h"
#define SceneActorRoute_GetStepDuration Route_GetCurrentPointNodeId
#include "global.h"
#include "scene_actor_control_phase.h"
u16 SceneActorRoute_GetStepDuration(void *);
void SceneActorRoute_AdvanceActorPoint(void *, void *);
void SceneActorRoute_UpdateTimedPhase(struct ChaseActor *actor)
{
    u8 *block = (u8 *)actor + 0x3D4;
    void *route = actor->base.body.currentRoute;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorTerrain_InstallControl(actor, *(void **)(block + 0x220), 5);
        *(s16 *)(block + 0x1F2) = SceneActorRoute_GetStepDuration(route);
    }
    if (*(s16 *)(block + 0x1F2) > 0)
        --*(u16 *)(block + 0x1F2);
    if (!(u8)Callback_InvokeActor(actor, actor->base.targetQuery) && actor->base.stepDone &&
        ActorStatus_BehaviorFlags(actor, 0x400000))
        SceneActorRoute_AdvanceActorPoint(actor, route);
}
