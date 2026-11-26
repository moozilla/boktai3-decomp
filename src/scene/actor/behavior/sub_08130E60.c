#include "scene/motion/scene_actor_route.h"
#include "scene/motion/scene_actor_chase_shared.h"
#define SceneActorRoute_GetStepDuration Route_GetCurrentPointNodeId
#include "global.h"
#include "scene/actor/scene_actor_control_phase.h"
s32 SceneActorRoute_GetStepDuration(void *);
u8 sub_0812F3CC(void *);
void SceneActorRoute_AdvanceActorPoint(void *, void *);
void SceneActorRoute_UpdateWaitPhase(struct ChaseActor *actor)
{
    struct SceneActorRouteCursor *route = (void *)&actor->group.homeRoute;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
    }
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        if (!sub_0812F3CC(actor)) {
            u32 kind = route->point->attributes >> 4;
            if (kind == 1) {
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = kind;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = kind;
                {
                    u32 *flags = &actor->base.statusFlags;
                    s32 mask = ~0x20;
                    *flags &= mask;
                }
            } else {
                u8 elapsed = route->unknown3++;
                if (elapsed >= SceneActorRoute_GetStepDuration(route))
                    SceneActorRoute_AdvanceActorPoint(actor, route);
                else
                    ++actor->base.counter2AC;
            }
        }
    }
}
