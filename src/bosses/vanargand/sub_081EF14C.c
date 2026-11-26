#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_UpdateIdleDisplay(void);

void SceneActors_EnterIdleState(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, SceneActors_UpdateIdleDisplay);
    }
    actor->actionTimer++;
}
