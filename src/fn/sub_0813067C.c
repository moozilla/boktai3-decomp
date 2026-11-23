#include "scene_actor_control_phase.h"
#include "scene_actor_chase_shared.h"
#include "global.h"
#include "scene_actor_reset.h"
void SceneActorControl_UpdateSavedControlPhase(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
    } else {
        if (!actor->base.counter2AC)
            Trigger_ConsumePending((u8 *)actor + 0xC, 1);
        ++actor->base.counter2AC;
    }
}
