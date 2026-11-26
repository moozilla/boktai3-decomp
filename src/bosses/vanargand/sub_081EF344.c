#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_UpdateLeftDeparture(void);

void VanargandHead_UpdateMainStateTransition(struct Slot7Transition *actor)
{
    if (Slot7_TakeActionEntry(actor) == TRUE) {
        Slot7_TransitionTo(actor, SceneActors_UpdateLeftDeparture);
    }
    actor->actionTimer++;
}
