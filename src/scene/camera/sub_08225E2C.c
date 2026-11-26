#include "scene/camera/scene_camera_controller.h"
#include "global.h"
void Camera_ClampCenteredPosition(struct SceneCameraController *bounds, s16 *position)
{
    s32 half = position[1] >> 1;
    s32 x = position[0] - half;
    s32 z = position[2] - half;
    s32 limit;
    limit = bounds->minBounds[0];
    if (x < limit)
        x = limit;
    else {
        limit = bounds->maxBounds[0];
        if (x > limit)
            x = limit;
    }
    limit = bounds->minBounds[1];
    if (z < limit)
        z = limit;
    else {
        limit = bounds->maxBounds[1];
        if (z > limit)
            z = limit;
    }
    position[0] = x + half;
    position[2] = z + half;
}
