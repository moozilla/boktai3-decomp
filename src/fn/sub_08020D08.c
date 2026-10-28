#include "global.h"
#include "scene_event_dispatch.h"
s32 SceneMessages_UnlinkReceiver(s32, s32);
s32 SceneMessages_UnregisterReceiver(s32 a)
{
    s32 r;
    if (gSceneEventDispatchState)
        r = SceneMessages_UnlinkReceiver(gSceneEventDispatchState, a);
    else
        r = -1;
    return r;
}
