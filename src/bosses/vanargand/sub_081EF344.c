#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_UpdateLeftDeparture(void);

void VanargandHead_UpdateMainStateTransition(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, SceneActors_UpdateLeftDeparture);
    }
    actor->actionTimer++;
}
