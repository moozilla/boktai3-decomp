#include "scene/camera/scene_camera_controller.h"
#include "engine/ram.h"
void Camera_SetSway(void *, s32, s32);

void sub_082265D8(void *target, s32 arg1, s32 arg2)
{
    struct SceneCameraController *camera = (void *)gActiveCamera;
    if (camera != NULL) {
        camera->completionMode = 0;
        Camera_SetSway(target, arg1, arg2);
    }
}
