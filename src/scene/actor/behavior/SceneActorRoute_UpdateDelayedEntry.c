#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_control_begin.h"
#include "global.h"
#include "scene/actor/scene_actor_control_phase.h"
u8 SceneActorReaction_BeginContactResponse(struct ChaseActor *);
// confidence=low: restores saved control, then enters route behavior two after sixty ticks.
void SceneActorRoute_UpdateDelayedEntry(struct ChaseActor *actor)
{
    u8 result;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        SceneActorControl_Begin(actor, actor->base.onStanding, 0, 0);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    result = SceneActorReaction_BeginContactResponse(actor);
    if (result)
        return;
    if ((s32)actor->base.counter2AC > 59)
        SceneActorTerrain_EnterState(actor, 0, 0, 2);
    else
        actor->base.counter2AC++;
}
