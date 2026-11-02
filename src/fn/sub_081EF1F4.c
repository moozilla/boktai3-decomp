#include "slot7_transition.h"

void SceneActors_UpdateMovingDisplay(void);

void SceneActors_EnterAlternateMovementState(struct Slot7Transition *actor)
{
    if (Slot7_TakeActionEntry(actor) == TRUE) {
        Slot7_TransitionTo(actor, SceneActors_UpdateMovingDisplay);
    }
    actor->actionTimer++;
}
