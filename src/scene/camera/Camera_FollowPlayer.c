#include "scene/camera/scene_camera_controller.h"
#include "global.h"
#include "engine/ram.h"
void Camera_NotifyMoveComplete(struct SceneCameraController *);
void Camera_FollowPlayer(s32 n)
{
    struct SceneCameraController *p = (void *)gActiveCamera;
    if (p) {
        if (n <= 1) {
            p->moving = 0;
            Camera_NotifyMoveComplete(p);
        } else {
            p->moving = 1;
            p->positionMode = 6;
            p->positionTimer = n;
        }
    }
}
