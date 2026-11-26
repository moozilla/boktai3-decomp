#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_UpdateMovingDisplay(void);

void SceneActors_EnterMovementState(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, SceneActors_UpdateMovingDisplay);
    }
    actor->actionTimer++;
}
