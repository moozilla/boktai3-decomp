#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
struct CameraTask {
    u8 unknown[0x20];
    u8 receiver[0x34];
};
void SceneMessages_UnregisterReceiver(void *);
s32 Camera_Destroy(struct CameraTask *camera)
{
    if (!(gEngineFlags & 0x800))
        SceneMessages_UnregisterReceiver(camera->receiver);
    gActiveCamera = 0;
    return 0;
}
