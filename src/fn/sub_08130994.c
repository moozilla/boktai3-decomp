#include "scene_actor_control_begin.h"
#include "scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene_actor_control_phase.h"
void SceneActorControl_OnIdle(void);
// confidence=low: starts the actor callback at 0x234 then consumes its trigger.
static inline void SetFlags(struct ChaseActor *actor, u32 mask)
{
    u32 value = mask;
    value |= actor->base.reactionFlags;
    actor->base.reactionFlags = value;
}
static inline void SetState(struct ChaseActor *actor, u32 state, u32 zero)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = zero;
    {
        typeof(actor->base.field2B1) *q1 = &actor->base.field2B1;
        *q1 = 1;
    }
}

void SceneActorControl_UpdateCallbackTrigger(struct ChaseActor *actor)
{
    u32 taken = SceneActorControl_TakeFlag(&actor->base.field2B1);
    if (taken) {
        SceneActorTerrain_InstallControl(actor, actor->base.onDefeat, 0x11);
    } else if (actor->base.stepDone) {
        Trigger_ConsumePending(((u8 *)actor) + 0xC, 1);
        SceneActorControl_Begin(actor, SceneActorControl_OnIdle, 0x10, taken);
        SetState(actor, 3, taken);
        SetFlags(actor, 0x1000);
    } else {
        actor->base.counter2AC++;
    }
}
