#include "global.h"

void SceneMessages_UnregisterReceiver(void *);
s32 Nidhoggr_Destroy(void *);

s32 Nidhoggr_DestroyCutscene(u8 *t)
{
    SceneMessages_UnregisterReceiver(t + 0x18);
    return Nidhoggr_Destroy(t);
}
