#include "slot7_transition.h"

void SceneActors_DestroyTask(void);

void SingletonSlot7_UpdateSecondaryDestroyTransition(struct Slot7Transition *actor)
{
    if (Slot7_TakeActionEntry(actor) == TRUE) {
        Slot7_TransitionTo(actor, SceneActors_DestroyTask);
    }
    actor->actionTimer++;
}
