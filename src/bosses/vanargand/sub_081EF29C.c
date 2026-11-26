#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_UpdateHitSensitiveDeparture(void);

void SceneActors_EnterSequenceTwoActionState(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, SceneActors_UpdateHitSensitiveDeparture);
    }
    actor->actionTimer++;
}
