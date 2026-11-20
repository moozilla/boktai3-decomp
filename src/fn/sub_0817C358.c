#include "state_actor_action_entry.h"

void BossSlot2_UpdateFlaggedRecovery(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p)) {
        u32 v = 0xb;
        p->actionAnimation = v;
    }
    p->actionTimer++;
}
