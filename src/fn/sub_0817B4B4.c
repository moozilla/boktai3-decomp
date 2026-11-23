#include "scene_volume.h"

void SceneActors_UpdateTransitionToState14(struct SceneVolumeActor *actor)
{
    if (SceneVolume_TakeQueued(actor)) {
        u32 state = 0xe;
        actor->state = state;
    }
    if (actor->pending)
        Trigger_ConsumePending(actor->trigger, 1);
    actor->actionTimer++;
}
