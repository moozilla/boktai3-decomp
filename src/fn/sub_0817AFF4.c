#include "scene_volume.h"

s32 SceneVolume_CanActivate(struct SceneVolumeActor *volume)
{
    if (!(volume->previousChoice == 2 || volume->previousChoice == 10))
        return 0;
    if (volume->decisionIndex != 4)
        return 0;
    if (!(volume->effectFlags & 7))
        return 0;
    if (volume->target->actionMode)
        return 0;
    if (volume->impactCount)
        return 0;
    return 1;
}
