#include "bosses/vanargand/vanargand_head_transition.h"

void VanargandHead_UpdateLeftDeparture(void);

void VanargandHead_UpdateMainStateTransition(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, VanargandHead_UpdateLeftDeparture);
    }
    actor->actionTimer++;
}
