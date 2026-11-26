#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_DestroyTask(void);

void VanargandHead_UpdateSecondaryDestroyTransition(struct Slot7Transition *actor)
{
    if (Slot7_TakeActionEntry(actor) == TRUE) {
        Slot7_TransitionTo(actor, SceneActors_DestroyTask);
    }
    actor->actionTimer++;
}
