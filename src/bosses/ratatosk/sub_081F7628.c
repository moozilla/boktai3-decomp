#include "global.h"
#include "bosses/state_actor_action_entry.h"

void SceneActor_UpdateState13Then29Command(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0xd);
    }
    if (p->actionFinished) {
        SceneActorState_SetAnimation(p, 0x1d);
        Trigger_ConsumePending(p->receiver2, 1);
    } else
        p->actionTimer++;
}
