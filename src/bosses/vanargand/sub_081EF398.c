#include "bosses/vanargand/vanargand_head_transition.h"

void VanargandHead_UpdatePaletteDispersal(void);

void VanargandHead_UpdateAlternateStateTransition(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, VanargandHead_UpdatePaletteDispersal);
    }
    actor->actionTimer++;
}
