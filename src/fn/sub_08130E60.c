#define SceneActorRoute_GetStepDuration Route_GetCurrentPointNodeId
#include "global.h"
#include "scene_actor_control_phase.h"
s32 SceneActorRoute_GetStepDuration(void *);
u8 sub_0812F3CC(void *);
void SceneActorRoute_AdvanceActorPoint(void *, void *);
void SceneActorRoute_UpdateWaitPhase(struct ChaseActor *actor)
{
    u8 *route = (u8 *)actor + 0x448;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        void *control = actor->base.onStanding;
        u32 zero = 0;
        actor->base.field2B2 = 1;
        actor->base.stepDone = zero;
        actor->base.controlKind = zero;
        actor->base.target = control;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = zero;
    }
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        u32 zero = sub_0812F3CC(actor);
        if (!zero) {
            u32 kind = (*(u8 **)(route + 8))[6] >> 4;
            if (kind == 1) {
                actor->base.state2A8 = zero;
                actor->base.phase2A9 = zero;
                actor->base.behavior2AA = kind;
                actor->base.counter2AC = zero;
                actor->base.field2B1 = kind;
                {
                    u32 *flags = &actor->base.statusFlags;
                    s32 mask = ~0x20;
                    *flags &= mask;
                }
            } else {
                u8 elapsed = route[3]++;
                if (elapsed >= SceneActorRoute_GetStepDuration(route))
                    SceneActorRoute_AdvanceActorPoint(actor, route);
                else
                    ++actor->base.counter2AC;
            }
        }
    }
}
