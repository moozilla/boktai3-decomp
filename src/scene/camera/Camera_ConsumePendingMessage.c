#include "scene/camera/scene_camera_controller.h"

void Camera_ConsumePendingMessage(struct SceneCameraController *camera)
{
    Trigger_ConsumePending(&camera->receiver.queue, 1);
}
