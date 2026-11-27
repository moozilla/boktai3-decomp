#include "scene/camera/scene_camera_controller.h"
#include "global.h"
#include "engine/ram.h"
void Camera_NotifyMoveComplete(struct SceneCameraController *);
void Camera_ReturnToDefault(s32 n)
{
    struct SceneCameraController *p = (void *)gActiveCamera;
    if (p) {
        p->moving = 1;
        if (n <= 1) {
            p->positionMode = 0;
            Camera_NotifyMoveComplete(p);
        } else {
            p->positionTimer = n;
            p->positionMode = 5;
        }
    }
}
