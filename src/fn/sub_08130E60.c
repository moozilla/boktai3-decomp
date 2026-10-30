#define SceneActorRoute_GetStepDuration Route_GetCurrentPointNodeId
#include "global.h"
#include "scene_actor_control_phase.h"
s32 SceneActorRoute_GetStepDuration(void *);
u8 sub_0812F3CC(void *);
void SceneActorRoute_AdvanceActorPoint(void *, void *);
void SceneActorRoute_UpdateWaitPhase(struct SceneActorRegistryResetView *actor)
{
    u8 *route = (u8 *)actor + 0x448;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *control = actor->savedTarget;
        u32 zero = 0;
        actor->field2B2 = 1;
        actor->field2B0 = zero;
        actor->field2B5 = zero;
        actor->target = control;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
    }
    if (!(u8)Callback_InvokeActor(actor, actor->contactQuery.opaque)) {
        u32 zero = sub_0812F3CC(actor);
        if (!zero) {
            u32 kind = (*(u8 **)(route + 8))[6] >> 4;
            if (kind == 1) {
                actor->state2A8 = zero;
                actor->phase2A9 = zero;
                actor->behavior2AA = kind;
                actor->counter2AC = zero;
                actor->field2B1 = kind;
                {
                    u32 *flags = &actor->flags2D0;
                    s32 mask = ~0x20;
                    *flags &= mask;
                }
            } else {
                u8 elapsed = route[3]++;
                if (elapsed >= SceneActorRoute_GetStepDuration(route))
                    SceneActorRoute_AdvanceActorPoint(actor, route);
                else
                    ++actor->counter2AC;
            }
        }
    }
}
