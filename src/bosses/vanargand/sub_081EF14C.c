#include "bosses/vanargand/vanargand_head_transition.h"

void VanargandHead_UpdateIdleDisplay(void);

void VanargandHead_EnterIdleState(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, VanargandHead_UpdateIdleDisplay);
    }
    actor->actionTimer++;
}
