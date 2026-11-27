#include "scene/motion/scene_actor_route.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_control_phase.h"
s32 Callback_InvokeActor(void *arg, void *function);
s32 Route_GetCurrentPointNodeId(void *);
u8 SceneActorReaction_BeginContactResponse(void *);
void SceneActorRoute_AdvanceActorPoint(void *, void *);
void SceneActorRoute_UpdateTriggeredWaitPhase(struct ChaseActor *actor)
{

    struct SceneActorRouteCursor *route = (void *)&actor->group.homeRoute;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorTerrain_InstallControl(actor, actor->base.onEntry, 2);
    }
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        if (!SceneActorReaction_BeginContactResponse(actor)) {
            u8 finished = actor->base.stepDone;
            u32 *flags = &actor->base.statusFlags;
            if (finished)
                SceneActorBehavior_SetWordFlags(flags, 0x20);
            if (SceneActorChase_Bits32(flags, 0x20)) {
                if (route->unknown3++ >= Route_GetCurrentPointNodeId(route)) {
                    SceneActorRoute_AdvanceActorPoint(actor, route);
                    return;
                }
            }
            ++actor->base.counter2AC;
        }
    }
}
