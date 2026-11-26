#include "bosses/vanargand/vanargand_head_transition.h"

void VanargandHead_UpdateMovingDisplay(void);

void VanargandHead_EnterAlternateMovementState(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, VanargandHead_UpdateMovingDisplay);
    }
    actor->actionTimer++;
}
