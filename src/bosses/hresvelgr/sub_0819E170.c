#include "bosses/state_actor_action_entry.h"

void sub_081A1D18(void *, int);

void ActorAction_WaitThenChangeBehavior(struct StateActor *p)
{
    if (StateActor_ConsumeActionEntry(p))
        SceneActorState_SetAnimation(p, 0x24);
    if (++p->actionTimer > 60) {
        SceneActorState_StartAction(p, 1, 0, 4);
        SceneActorState_EnterAction(p, sub_081A1D18, 0x25);
    }
}
