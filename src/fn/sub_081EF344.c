#include "slot7_transition.h"

void SceneActors_UpdateLeftDeparture(void);

void SingletonSlot7_UpdateMainStateTransition(struct Slot7Transition *actor)
{
    if (Slot7_TakeActionEntry(actor) == TRUE) {
        Slot7_TransitionTo(actor, SceneActors_UpdateLeftDeparture);
    }
    actor->actionTimer++;
}
