#include "slot7_transition.h"

void SceneActors_UpdateDelayedMovement(void);

void SceneActors_EnterDelayedMovementState(struct Slot7Transition *actor)
{
    if (Slot7_TakeActionEntry(actor) == TRUE) {
        Slot7_TransitionTo(actor, SceneActors_UpdateDelayedMovement);
    }
    actor->actionTimer++;
}
