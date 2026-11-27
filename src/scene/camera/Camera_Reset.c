#include "scene/camera/scene_camera_controller.h"
#include "engine/ram.h"
void Camera_ClearTargetOffset(void);

void Camera_Reset(void)
{
    struct SceneCameraController *camera = (void *)gActiveCamera;
    if (camera) {
        camera->moving = 1;
        camera->positionMode = 0;
        Camera_ClearTargetOffset();
    }
}
