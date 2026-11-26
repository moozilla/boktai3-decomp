#include "bosses/sabata/sabata_attack_state.h"

void SceneActors_UpdateTransitionToState16(struct SabataAttackActor *actor)
{
    if (Sabata_TakeQueued(actor)) {
        u32 state = 0x10;
        actor->state = state;
    }
    if (actor->pending)
        Trigger_ConsumePending(&actor->receiver, 1);
    actor->actionTimer++;
}
