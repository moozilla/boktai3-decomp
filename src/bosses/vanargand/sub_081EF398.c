#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_UpdatePaletteDispersal(void);

void VanargandHead_UpdateAlternateStateTransition(struct Slot7Transition *actor)
{
    if (Slot7_TakeActionEntry(actor) == TRUE) {
        Slot7_TransitionTo(actor, SceneActors_UpdatePaletteDispersal);
    }
    actor->actionTimer++;
}
