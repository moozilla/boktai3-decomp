#include "bosses/vanargand/vanargand_head_transition.h"

void VanargandHead_UpdatePeriodicEffectAction(void);

void VanargandHead_EnterRandomizedActionState(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, VanargandHead_UpdatePeriodicEffectAction);
    }
    actor->actionTimer++;
}
