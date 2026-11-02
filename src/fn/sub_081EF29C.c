#include "slot7_transition.h"

void SceneActors_UpdateHitSensitiveDeparture(void);

void SceneActors_EnterSequenceTwoActionState(struct Slot7Transition *actor)
{
    if (Slot7_TakeActionEntry(actor) == TRUE) {
        Slot7_TransitionTo(actor, SceneActors_UpdateHitSensitiveDeparture);
    }
    actor->actionTimer++;
}
