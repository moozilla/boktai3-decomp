#include "scene/camera/scene_camera_controller.h"
#include "engine/ram.h"

void Camera_FollowPlayer(u32);

void Camera_FollowPlayerWithoutCompletion(u32 a)
{
    struct SceneCameraController *camera = (void *)gActiveCamera;
    if (camera != NULL) {
        camera->completionMode = 0;
        Camera_FollowPlayer(a);
    }
}
