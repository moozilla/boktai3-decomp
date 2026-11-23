#include "scene_volume.h"

s32 SceneVolume_ConsumePendingEvent(struct SceneVolumeActor *volume)
{
    s32 result;
    if (volume->reached != 1) {
        result = 0;
    } else {
        volume->reached = 0;
        result = 1;
    }
    return result;
}
