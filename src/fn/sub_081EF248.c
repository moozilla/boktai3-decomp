#include "slot7_transition.h"

void SceneActors_UpdatePeriodicEffectAction(void);

void SceneActors_EnterRandomizedActionState(struct Slot7Transition *actor)
{
    if (Slot7_TakeActionEntry(actor) == TRUE) {
        Slot7_TransitionTo(actor, SceneActors_UpdatePeriodicEffectAction);
    }
    actor->actionTimer++;
}
