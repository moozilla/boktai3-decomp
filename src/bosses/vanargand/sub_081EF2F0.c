#include "bosses/vanargand/vanargand_head_transition.h"

void VanargandHead_UpdateDelayedMovement(void);

void VanargandHead_EnterDelayedMovementState(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, VanargandHead_UpdateDelayedMovement);
    }
    actor->actionTimer++;
}
