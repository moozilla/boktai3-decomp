#include "bosses/vanargand/vanargand_head_transition.h"

void SceneActors_UpdatePeriodicEffectAction(void);

void SceneActors_EnterRandomizedActionState(struct VanargandHeadTransition *actor)
{
    if (VanargandHead_TakeActionEntry(actor) == TRUE) {
        VanargandHead_TransitionTo(actor, SceneActors_UpdatePeriodicEffectAction);
    }
    actor->actionTimer++;
}
