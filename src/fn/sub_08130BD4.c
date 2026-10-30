#define SceneActorRoute_GetStepDuration Route_GetCurrentPointNodeId
#include "global.h"
#include "scene_actor_control_phase.h"
u16 SceneActorRoute_GetStepDuration(void *);
void SceneActorRoute_AdvanceActorPoint(void *, void *);
static inline u32 TestFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
void SceneActorRoute_UpdateTimedPhase(struct SceneActorRegistryResetView *actor)
{
    u8 *block = (u8 *)actor + 0x3D4;
    void *route = actor->body.currentRoute;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *control = *(void **)(block + 0x220);
        u32 kind = 5;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = control;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        *(s16 *)(block + 0x1F2) = SceneActorRoute_GetStepDuration(route);
    }
    if (*(s16 *)(block + 0x1F2) > 0)
        --*(u16 *)(block + 0x1F2);
    if (!(u8)Callback_InvokeActor(actor, actor->update26C) && actor->field2B0 &&
        TestFlags(actor, 0x400000))
        SceneActorRoute_AdvanceActorPoint(actor, route);
}
