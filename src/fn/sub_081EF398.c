#include "slot7_transition.h"

void SceneActors_UpdatePaletteDispersal(void);

void SingletonSlot7_UpdateAlternateStateTransition(struct Slot7Transition *actor)
{
    if (Slot7_TakeActionEntry(actor) == TRUE) {
        Slot7_TransitionTo(actor, SceneActors_UpdatePaletteDispersal);
    }
    actor->actionTimer++;
}
