#include "engine/shared/state_actor_action_entry.h"

void BossSlot2_UpdateFlaggedRecovery(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 11);
    p->actionTimer++;
}
