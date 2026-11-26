#include "bosses/sabata/scene_volume.h"

s32 SceneVolume_GetActivityState(struct SceneVolumeActor *volume)
{
    s32 result;
    if (volume->activityTimer > 0) {
    active:
        return 1;
    }
    {
        u32 mask = 1;
        if (mask & volume->flags) {
            if (volume->activity == 0)
                goto active;
            result = 2;
        } else {
            result = 0;
        }
    }
    return result;
}
