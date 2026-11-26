#include "scene/camera/scene_camera_controller.h"
#include "global.h"
void Camera_ClampCenteredPosition(struct SceneCameraController *, s16 *);
void Camera_ClampDiagonalPosition(struct SceneCameraController *, s16 *);
void Camera_ApplyPositionBounds(struct SceneCameraController *camera, s16 *position)
{
    if (camera->boundsMode == 1)
        Camera_ClampCenteredPosition(camera, position);
    else if (camera->boundsMode == 2)
        Camera_ClampDiagonalPosition(camera, position);
}
