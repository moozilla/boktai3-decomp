#include "bosses/state_actor_action_entry.h"
void Hresvelgr_UpdateActionTwentyEightRequest(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        SceneActorState_SetAnimation(p, 0x1c);
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}
