#include "scene/camera/scene_camera_controller.h"
#include "engine/ram.h"

s32 Camera_Destroy(struct SceneCameraController *camera)
{
    if (!(gEngineFlags & 0x800))
        SceneMessages_UnregisterReceiver(&camera->receiver);
    gActiveCamera = 0;
    return 0;
}
