#include "scene/camera/scene_camera_controller.h"
#include "engine/ram.h"
void Camera_Reset(void);

void sub_0822624C(void)
{
    struct SceneCameraController *camera = (void *)gActiveCamera;
    if (camera != NULL) {
        camera->completionMode = 0;
        Camera_Reset();
    }
}
