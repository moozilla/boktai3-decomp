#include "scene/camera/scene_camera_controller.h"
#include "engine/ram.h"
void Camera_ReturnToDefault(void *);

void sub_082264B0(void *target)
{
    struct SceneCameraController *camera = (void *)gActiveCamera;
    if (camera != NULL) {
        camera->completionMode = 0;
        Camera_ReturnToDefault(target);
    }
}
