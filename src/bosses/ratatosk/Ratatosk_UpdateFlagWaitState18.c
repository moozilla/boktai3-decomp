#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "bosses/state_actor_action_entry.h"

void Ratatosk_UpdateFlagWaitState18(struct StateActor *actor)
{
    struct Ratatosk *p = (struct Ratatosk *)actor;
    if (StateActor_ConsumeActionEntry(actor))
        SceneActorState_SetAnimation(actor, 0x12);
    if (!SceneActorState_TestFlags(actor, 1)) {
        p->interval = 1;
        SceneActorState_StartAction(actor, 0, 0, 1);
    } else
        actor->actionTimer++;
}
