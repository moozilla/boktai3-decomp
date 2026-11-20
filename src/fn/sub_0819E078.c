#include "state_actor_action_entry.h"
void Hresvelgr_UpdateActionTwentyEightRequest(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        u32 v = 0x1c;
        p->actionAnimation = v;
        Trigger_ConsumePending(p->receiver2, 1);
    }
    p->actionTimer++;
}
