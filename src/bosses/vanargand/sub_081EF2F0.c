#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_UpdateDelayedMovement(void);

void SceneActors_EnterDelayedMovementState(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, SceneActors_UpdateDelayedMovement);
    }
    actor->actionTimer++;
}
