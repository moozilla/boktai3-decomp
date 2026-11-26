#include "scene/camera/scene_camera_controller.h"
#include "script/script_utility.h"

void Camera_RunCompletionScript(struct SceneCameraController *camera)
{
    u32 script = camera->rate;
    if (script != 0) {
        if (camera->rateIsValue == 0) {
            camera->rate = 0;
            Script_RunBlock(script, NULL);
        } else {
            camera->rate = 0;
            Script_Call(script, NULL);
        }
    }
}
