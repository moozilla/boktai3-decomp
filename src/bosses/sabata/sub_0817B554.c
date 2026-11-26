#include "bosses/sabata/scene_volume.h"

void SceneActors_UpdateTransitionToState16(struct SceneVolumeActor *actor)
{
    if (SceneVolume_TakeQueued(actor)) {
        u32 state = 0x10;
        actor->state = state;
    }
    if (actor->pending)
        Trigger_ConsumePending(&actor->receiver, 1);
    actor->actionTimer++;
}
