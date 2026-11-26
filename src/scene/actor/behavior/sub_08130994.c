#include "scene/actor/scene_actor_control_begin.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene/actor/scene_actor_control_phase.h"
void SceneActorControl_OnIdle(void);
// confidence=low: starts the actor callback at 0x234 then consumes its trigger.
static inline void SetFlags(struct ChaseActor *actor, u32 mask)
{
    u32 value = mask;
    value |= actor->base.reactionFlags;
    actor->base.reactionFlags = value;
}
void SceneActorControl_UpdateCallbackTrigger(struct ChaseActor *actor)
{
    u32 taken = SceneActorControl_TakeFlag(&actor->base.field2B1);
    if (taken) {
        SceneActorTerrain_InstallControl(actor, actor->base.onDefeat, 0x11);
    } else if (actor->base.stepDone) {
        Trigger_ConsumePending(((u8 *)actor) + 0xC, 1);
        SceneActorControl_Begin(actor, SceneActorControl_OnIdle, 0x10, taken);
        SceneActorTerrain_EnterState(actor, 3, 0, 0);
        SetFlags(actor, 0x1000);
    } else {
        actor->base.counter2AC++;
    }
}
