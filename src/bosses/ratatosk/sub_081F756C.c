#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "bosses/state_actor_action_entry.h"

void SceneActor_UpdateState24AndConditionalReturnCommand(struct StateActor *actor)
{
    struct Ratatosk *p = (struct Ratatosk *)actor;
    if (StateActor_ConsumeActionEntry(actor))
        SceneActorState_SetAnimation(actor, 0x18);
    if (actor->actionFinished) {
        if (p->stateParameter == 0)
            SceneActorState_SetAnimation(actor, 1);
        Trigger_ConsumePending(actor->receiver2, 1);
    } else
        actor->actionTimer++;
}
