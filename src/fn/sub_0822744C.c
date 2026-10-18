#define Camera_Destroy                   sub_0822744C
#define gActiveCamera                    gUnk_03005420
#include "global.h"
struct CameraTask {
    u8 unknown[0x20];
    u8 receiver[0x34];
};
extern u32 gUnk_030053F4;
extern struct CameraTask *gUnk_03005420;
void SceneMessages_UnregisterReceiver(void *);
s32 sub_0822744C(struct CameraTask *camera)
{
    if (!(gUnk_030053F4 & 0x800))
        SceneMessages_UnregisterReceiver(camera->receiver);
    gUnk_03005420 = 0;
    return 0;
}
