#include "scene/camera/scene_camera_controller.h"
#include "global.h"
void Camera_RunCompletionScript(struct SceneCameraController *);
void Camera_ConsumePendingMessage(struct SceneCameraController *);
void Camera_NotifyMoveComplete(struct SceneCameraController *p)
{
    u32 v = p->completionMode;
    if (v == 1)
        Camera_RunCompletionScript(p);
    else if (v == 2)
        Camera_ConsumePendingMessage(p);
}
