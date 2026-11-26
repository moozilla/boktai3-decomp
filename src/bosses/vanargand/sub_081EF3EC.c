#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_DestroyTask(void);

void VanargandHead_UpdateDestroyTransition(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, SceneActors_DestroyTask);
    }
    actor->actionTimer++;
}
