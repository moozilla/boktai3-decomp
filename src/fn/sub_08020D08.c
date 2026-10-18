#include "global.h"
extern s32 gUnk_0200047C;
s32 SceneMessages_UnlinkReceiver(s32, s32);
s32 SceneMessages_UnregisterReceiver(s32 a)
{
    s32 r;
    if (gUnk_0200047C)
        r = SceneMessages_UnlinkReceiver(gUnk_0200047C, a);
    else
        r = -1;
    return r;
}
