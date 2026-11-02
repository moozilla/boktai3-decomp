#include "slot7_transition.h"

void SceneActors_UpdateIdleDisplay(void);

void SceneActors_EnterIdleState(struct Slot7Transition *actor)
{
    if (Slot7_TakeActionEntry(actor) == TRUE) {
        Slot7_TransitionTo(actor, SceneActors_UpdateIdleDisplay);
    }
    actor->actionTimer++;
}
