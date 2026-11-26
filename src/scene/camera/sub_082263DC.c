#include "scene/camera/scene_camera_controller.h"
#include "engine/ram.h"
void Camera_TrackActor(void *, s32);

void sub_082263DC(void *target, s32 arg)
{
    struct SceneCameraController *camera = (void *)gActiveCamera;
    if (camera != NULL) {
        camera->completionMode = 0;
        Camera_TrackActor(target, arg);
    }
}
