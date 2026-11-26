#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_UpdatePaletteDispersal(void);

void VanargandHead_UpdateAlternateStateTransition(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, SceneActors_UpdatePaletteDispersal);
    }
    actor->actionTimer++;
}
