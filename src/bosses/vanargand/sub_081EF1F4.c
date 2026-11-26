#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_UpdateMovingDisplay(void);

void SceneActors_EnterAlternateMovementState(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, SceneActors_UpdateMovingDisplay);
    }
    actor->actionTimer++;
}
