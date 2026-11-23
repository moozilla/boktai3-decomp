#include "scene_volume.h"

void SceneActors_UpdateTransitionToState15(struct SceneVolumeActor *actor)
{
    if (SceneVolume_TakeQueued(actor)) {
        u32 state = 0xF;
        actor->state = state;
    }
    if (actor->pending)
        Trigger_ConsumePending(actor->trigger, 1);
    actor->actionTimer++;
}
