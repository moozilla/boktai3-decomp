#include "bosses/vanargand/vanargand_head_transition.h"

void VanargandHead_UpdateHitSensitiveDeparture(void);

void VanargandHead_EnterSequenceTwoActionState(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, VanargandHead_UpdateHitSensitiveDeparture);
    }
    actor->actionTimer++;
}
